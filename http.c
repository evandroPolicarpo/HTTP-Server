#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "http.h"

/* Copia uma string para memória recém-alocada (equivalente a strdup). */
static char *http_strdup(const char *s){
    size_t len;
    char *copy;

    if(s == NULL) return NULL;

    len = strlen(s) + 1;
    copy = malloc(len);
    if(copy == NULL) return NULL;

    memcpy(copy, s, len);
    return copy;
}

void http_message_init(http_message_t *msg){
    if(msg == NULL) return;

    msg->method[0] = '\0';
    msg->version[0] = '\0';
    msg->url[0] = '\0';
    msg->headers.items = NULL;
    msg->headers.count = 0;
    msg->headers.capacity = 0;
    msg->body = NULL;
    msg->body_len = 0;
}

void http_message_free(http_message_t *msg){
    if(msg == NULL) return;

    http_headers_free(&msg->headers);
    free(msg->body);
    msg->body = NULL;
    msg->body_len = 0;
}

void http_message_print(const http_message_t *msg){
    size_t i;

    if(msg == NULL){
        printf("(mensagem nula)\n");
        return;
    }

    printf("Método : %s\n", msg->method[0] != '\0' ? msg->method : "(vazio)");
    printf("URL    : %s\n", msg->url[0] != '\0' ? msg->url : "(vazia)");
    printf("Versão : %s\n", msg->version[0] != '\0' ? msg->version : "(vazia)");

    printf("Headers (%zu):\n", msg->headers.count);
    if(msg->headers.count == 0){
        printf("  (nenhum)\n");
    } else {
        for(i = 0; i < msg->headers.count; i++){
            printf("  %s: %s\n",
                   msg->headers.items[i].name,
                   msg->headers.items[i].value);
        }
    }

    printf("Body (%zu bytes):\n", msg->body_len);
    if(msg->body == NULL || msg->body_len == 0){
        printf("  (vazio)\n");
    } else {
        printf("  ");
        fwrite(msg->body, 1, msg->body_len, stdout);
        if(msg->body[msg->body_len - 1] != '\n') printf("\n");
    }
}

int http_headers_add(http_headers_t *headers, const char *name, const char *value){
    http_header_t *new_items;
    size_t new_capacity;
    char *name_copy;
    char *value_copy;

    if(headers == NULL || name == NULL || value == NULL) return -1;

    /* Cresce o vetor quando ele estiver cheio. */
    if(headers->count == headers->capacity){
        new_capacity = (headers->capacity == 0) ? 8 : headers->capacity * 2;

        new_items = realloc(headers->items, new_capacity * sizeof(*new_items));
        if(new_items == NULL) return -1;

        headers->items = new_items;
        headers->capacity = new_capacity;
    }

    name_copy = http_strdup(name);
    if(name_copy == NULL) return -1;

    value_copy = http_strdup(value);
    if(value_copy == NULL){
        free(name_copy);
        return -1;
    }

    headers->items[headers->count].name = name_copy;
    headers->items[headers->count].value = value_copy;
    headers->count++;

    return 0;
}

const char *http_headers_get(const http_headers_t *headers, const char *name){
    size_t i;

    if(headers == NULL || name == NULL) return NULL;

    for(i = 0; i < headers->count; i++){
        if(headers->items[i].name != NULL &&
           strcasecmp(headers->items[i].name, name) == 0){
            return headers->items[i].value;
        }
    }

    return NULL;
}

int http_headers_remove(http_headers_t *headers, const char *name){
    size_t i;

    if(headers == NULL || name == NULL) return -1;

    for(i = 0; i < headers->count; i++){
        if(headers->items[i].name != NULL &&
           strcasecmp(headers->items[i].name, name) == 0){
            size_t remaining = headers->count - i - 1;

            free(headers->items[i].name);
            free(headers->items[i].value);

            /* Mantém o vetor compacto deslocando os itens seguintes. */
            if(remaining > 0){
                memmove(&headers->items[i], &headers->items[i + 1],
                        remaining * sizeof(*headers->items));
            }

            headers->count--;
            return 0;
        }
    }

    return -1;
}

void http_headers_free(http_headers_t *headers){
    size_t i;

    if(headers == NULL) return;

    for(i = 0; i < headers->count; i++){
        free(headers->items[i].name);
        free(headers->items[i].value);
    }

    free(headers->items);
    headers->items = NULL;
    headers->count = 0;
    headers->capacity = 0;
}

int http_message_set_body(http_message_t *msg, const char *body, size_t len){
    char *new_body;

    if(msg == NULL) return -1;

    free(msg->body);
    msg->body = NULL;
    msg->body_len = 0;

    if(body == NULL || len == 0) return 0;

    new_body = malloc(len + 1);
    if(new_body == NULL) return -1;

    memcpy(new_body, body, len);
    new_body[len] = '\0';

    msg->body = new_body;
    msg->body_len = len;

    return 0;
}
