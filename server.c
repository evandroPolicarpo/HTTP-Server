#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "parser.h"

#define DEFAULT_PORT       8080
#define LISTEN_BACKLOG     10
#define REQUEST_INITIAL    4096
#define REQUEST_MAX        (1024 * 1024)

/* Envia todos os bytes de `buffer`, tratando envios parciais. */
static int send_all(int fd, const char *buffer, size_t length){
    size_t sent = 0;

    while(sent < length){
        ssize_t n = send(fd, buffer + sent, length - sent, 0);

        if(n < 0){
            if(errno == EINTR) continue;
            return -1;
        }
        if(n == 0) return -1;

        sent += (size_t)n;
    }

    return 0;
}

/* Procura o final dos headers (\r\n\r\n) e retorna o índice após ele. */
static ssize_t find_header_end(const char *buffer, size_t length){
    size_t i;

    for(i = 0; i + 3 < length; i++){
        if(buffer[i] == '\r' && buffer[i + 1] == '\n' &&
           buffer[i + 2] == '\r' && buffer[i + 3] == '\n'){
            return (ssize_t)(i + 4);
        }
    }

    return -1;
}

/*
 * Lê o Content-Length diretamente dos headers já recebidos.
 * Retorna 0 em sucesso (inclusive quando o header não existe) e -1 se o
 * valor for inválido.
 */
static int get_content_length(const char *buffer, size_t header_end,
                              size_t *content_length){
    static const char name[] = "Content-Length:";
    const size_t name_len = sizeof(name) - 1;
    size_t i = 0;

    *content_length = 0;

    /* Pula a linha de requisição. */
    while(i < header_end && buffer[i] != '\n') i++;
    if(i < header_end) i++;

    while(i < header_end){
        size_t line_start = i;
        size_t line_len;
        size_t value_len;
        const char *value;
        char value_copy[32];
        char *end = NULL;
        unsigned long long parsed;

        while(i < header_end && buffer[i] != '\r' && buffer[i] != '\n') i++;
        line_len = i - line_start;

        if(line_len == 0) return 0; /* Linha em branco: fim dos headers. */

        if(line_len >= name_len &&
           strncasecmp(&buffer[line_start], name, name_len) == 0){
            value = &buffer[line_start + name_len];
            value_len = line_len - name_len;

            while(value_len > 0 && (*value == ' ' || *value == '\t')){
                value++;
                value_len--;
            }
            while(value_len > 0 &&
                  (value[value_len - 1] == ' ' || value[value_len - 1] == '\t')){
                value_len--;
            }

            if(value_len == 0 || value_len >= sizeof(value_copy)) return -1;

            memcpy(value_copy, value, value_len);
            value_copy[value_len] = '\0';

            parsed = strtoull(value_copy, &end, 10);
            if(end == value_copy || *end != '\0') return -1;

            *content_length = (size_t)parsed;
            return 0;
        }

        /* Avança o CRLF da linha. */
        if(i < header_end && buffer[i] == '\r') i++;
        if(i < header_end && buffer[i] == '\n') i++;
    }

    return 0;
}

/*
 * Lê uma requisição inteira do socket, incluindo o body quando houver
 * Content-Length. Retorna um buffer terminado em '\0' ou NULL em erro.
 */
static char *read_request(int fd, size_t *request_len){
    size_t capacity = REQUEST_INITIAL;
    size_t length = 0;
    size_t header_end = 0;
    size_t content_length = 0;
    int headers_complete = 0;
    char *buffer = malloc(capacity);

    if(buffer == NULL) return NULL;

    for(;;){
        ssize_t n;

        if(length + 1 >= capacity){
            size_t new_capacity;
            char *new_buffer;

            if(capacity >= REQUEST_MAX){
                free(buffer);
                return NULL;
            }

            new_capacity = capacity * 2;
            if(new_capacity > REQUEST_MAX) new_capacity = REQUEST_MAX;

            new_buffer = realloc(buffer, new_capacity);
            if(new_buffer == NULL){
                free(buffer);
                return NULL;
            }

            buffer = new_buffer;
            capacity = new_capacity;
        }

        n = recv(fd, buffer + length, capacity - length - 1, 0);
        if(n < 0){
            if(errno == EINTR) continue;
            free(buffer);
            return NULL;
        }
        if(n == 0) break;

        length += (size_t)n;
        buffer[length] = '\0';

        if(!headers_complete){
            ssize_t end = find_header_end(buffer, length);

            if(end >= 0){
                headers_complete = 1;
                header_end = (size_t)end;

                if(get_content_length(buffer, header_end, &content_length) != 0){
                    free(buffer);
                    return NULL;
                }
            }
        }

        if(headers_complete && length >= header_end + content_length) break;
    }

    if(!headers_complete || length < header_end + content_length){
        free(buffer);
        return NULL;
    }

    *request_len = length;
    return buffer;
}

static void send_error(int fd, int status, const char *reason){
    char response[256];
    int length = snprintf(response, sizeof(response),
                          "HTTP/1.1 %d %s\r\n"
                          "Content-Type: text/plain; charset=utf-8\r\n"
                          "Content-Length: %zu\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "%s\n",
                          status, reason, strlen(reason) + 1, reason);

    if(length > 0){
        send_all(fd, response, (size_t)length);
    }
}

static void handle_client(int client_fd, const char *client_ip){
    http_message_t msg;
    char *request;
    size_t request_len = 0;

    request = read_request(client_fd, &request_len);

    if(request == NULL){
        fprintf(stderr, "Requisição inválida ou incompleta de %s\n", client_ip);
        send_error(client_fd, 400, "Bad Request");
        return;
    }

    if(http_parser(request, &msg) != 0){
        fprintf(stderr, "Falha no parsing da requisição de %s\n", client_ip);
        send_error(client_fd, 400, "Bad Request");
        free(request);
        return;
    }

    printf("\n=== Requisição recebida de %s (%zu bytes) ===\n",
           client_ip, request_len);
    http_message_print(&msg);
    printf("==============================================\n");
    fflush(stdout);

    send_error(client_fd, 200, "OK");

    http_message_free(&msg);
    free(request);
}

int main(int argc, char **argv){
    int port = DEFAULT_PORT;
    int server_fd;
    struct sockaddr_in address;
    int option = 1;

    if(argc > 2){
        fprintf(stderr, "Uso: %s [porta]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argc == 2){
        char *end = NULL;
        long value = strtol(argv[1], &end, 10);

        if(end == argv[1] || *end != '\0' || value < 1 || value > 65535){
            fprintf(stderr, "Porta inválida: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        port = (int)value;
    }

    signal(SIGPIPE, SIG_IGN);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        perror("socket");
        return EXIT_FAILURE;
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                  &option, sizeof(option)) < 0){
        perror("setsockopt");
        close(server_fd);
        return EXIT_FAILURE;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons((uint16_t)port);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind");
        close(server_fd);
        return EXIT_FAILURE;
    }

    if(listen(server_fd, LISTEN_BACKLOG) < 0){
        perror("listen");
        close(server_fd);
        return EXIT_FAILURE;
    }

    printf("Servidor HTTP escutando na porta %d...\n", port);
    fflush(stdout);

    for(;;){
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);
        char client_ip[INET_ADDRSTRLEN];
        int client_fd;

        client_fd = accept(server_fd, (struct sockaddr *)&client_address,
                           &client_len);
        if(client_fd < 0){
            if(errno == EINTR) continue;
            perror("accept");
            continue;
        }

        if(inet_ntop(AF_INET, &client_address.sin_addr,
                     client_ip, sizeof(client_ip)) == NULL){
            strcpy(client_ip, "desconhecido");
        }

        handle_client(client_fd, client_ip);
        close(client_fd);
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
