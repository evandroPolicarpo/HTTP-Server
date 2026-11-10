#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

static int http_parser_method(const char *req, int *index, http_message_t *msg){
    AFD_STATE state = Q_METHOD;
    int i = *index;
    char c;

    while((c = req[i]) != '\0' && state != Q_ERROR && state != Q_URL){
        switch (state){
            case Q_METHOD:
                if(c == 'C') state = Q_METHOD_CONNECT_1;
                else if(c == 'D') state = Q_METHOD_DELETE_1;
                else if(c == 'G') state = Q_METHOD_GET_1;
                else if(c == 'H') state = Q_METHOD_HEAD_1;
                else if(c == 'O') state = Q_METHOD_OPTIONS_1;
                else if(c == 'P') state = Q_METHOD_P_1;
                else if(c == 'Q') state = Q_METHOD_QUERY_1;
                else if(c == 'T') state = Q_METHOD_TRACE_1;
                else state = Q_ERROR;
                break;

            //Letter 'P' Fork (PUT, PATCH or POST)
            case Q_METHOD_P_1:
                if(c == 'A') state = Q_METHOD_PATCH_2;
                else if(c == 'O') state = Q_METHOD_POST_2;
                else if(c == 'U') state = Q_METHOD_PUT_2;
                else state = Q_ERROR;
                break;


            //CONNECT Branch
            case Q_METHOD_CONNECT_1:
                if(c == 'O') state = Q_METHOD_CONNECT_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_CONNECT_2:
                if(c == 'N') state = Q_METHOD_CONNECT_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_CONNECT_3:
                if(c == 'N') state = Q_METHOD_CONNECT_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_CONNECT_4:
                if(c == 'E') state = Q_METHOD_CONNECT_5;
                else state = Q_ERROR;
                break;

            case Q_METHOD_CONNECT_5:
                if(c == 'C') state = Q_METHOD_CONNECT_6;
                else state = Q_ERROR;
                break;

            case Q_METHOD_CONNECT_6:
                if(c == 'T'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "CONNECT");
                }
                else state = Q_ERROR;
                break;

            //DELETE Branch
            case Q_METHOD_DELETE_1:
                if(c == 'E') state = Q_METHOD_DELETE_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_DELETE_2:
                if(c == 'L') state = Q_METHOD_DELETE_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_DELETE_3:
                if(c == 'E') state = Q_METHOD_DELETE_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_DELETE_4:
                if(c == 'T') state = Q_METHOD_DELETE_5;
                else state = Q_ERROR;
                break;

            case Q_METHOD_DELETE_5:
                if(c == 'E'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "DELETE");
                }
                else state = Q_ERROR;
                break;

            //GET Branch
            case Q_METHOD_GET_1:
                if(c == 'E') state = Q_METHOD_GET_2;
                else state = Q_ERROR;
                break;
            
            case Q_METHOD_GET_2:
                if(c == 'T'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "GET");
                }
                else state = Q_ERROR;
                break;

            //HEAD Branch
            case Q_METHOD_HEAD_1:
                if(c == 'E') state = Q_METHOD_HEAD_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_HEAD_2:
                if(c == 'A') state = Q_METHOD_HEAD_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_HEAD_3:
                if(c == 'D'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "HEAD");
                }
                else state = Q_ERROR;
                break;

            //OPTIONS Branch
            case Q_METHOD_OPTIONS_1:
                if(c == 'P') state = Q_METHOD_OPTIONS_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_OPTIONS_2:
                if(c == 'T') state = Q_METHOD_OPTIONS_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_OPTIONS_3:
                if(c == 'I') state = Q_METHOD_OPTIONS_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_OPTIONS_4:
                if(c == 'O') state = Q_METHOD_OPTIONS_5;
                else state = Q_ERROR;
                break;

            case Q_METHOD_OPTIONS_5:
                if(c == 'N') state = Q_METHOD_OPTIONS_6;
                else state = Q_ERROR;
                break;

            case Q_METHOD_OPTIONS_6:
                if(c == 'S'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "OPTIONS");
                }
                else state = Q_ERROR;
                break;

            //PATCH Branch
            case Q_METHOD_PATCH_2:
                if(c == 'T') state = Q_METHOD_PATCH_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_PATCH_3:
                if(c == 'C') state = Q_METHOD_PATCH_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_PATCH_4:
                if(c == 'H'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "PATCH");
                }
                else state = Q_ERROR;
                break;

            //POST Branch
            case Q_METHOD_POST_2:
                if(c == 'S') state = Q_METHOD_POST_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_POST_3:
                if(c == 'T'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "POST");
                }
                else state = Q_ERROR;
                break;

            //PUT Branch
            case Q_METHOD_PUT_2:
                if(c == 'T'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "PUT");
                }
                else state = Q_ERROR;
                break;

            //QUERY Branch
            case Q_METHOD_QUERY_1:
                if(c == 'U') state = Q_METHOD_QUERY_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_QUERY_2:
                if(c == 'E') state = Q_METHOD_QUERY_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_QUERY_3:
                if(c == 'R') state = Q_METHOD_QUERY_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_QUERY_4:
                if(c == 'Y'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "QUERY");
                }
                else state = Q_ERROR;
                break;

            //TRACE Branch
            case Q_METHOD_TRACE_1:
                if(c == 'R') state = Q_METHOD_TRACE_2;
                else state = Q_ERROR;
                break;

            case Q_METHOD_TRACE_2:
                if(c == 'A') state = Q_METHOD_TRACE_3;
                else state = Q_ERROR;
                break;

            case Q_METHOD_TRACE_3:
                if(c == 'C') state = Q_METHOD_TRACE_4;
                else state = Q_ERROR;
                break;

            case Q_METHOD_TRACE_4:
                if(c == 'E'){
                    state = Q_METHOD_END;
                    strcpy(msg->method, "TRACE");
                }
                else state = Q_ERROR;
                break;

            //Final Branch
            case Q_METHOD_END:
                if(c == ' ') state = Q_URL;
                else state = Q_ERROR;
                break;

            default:
                state = Q_ERROR;
                break;
        }

        i++;
    }

    *index = i;
    return (state == Q_URL) ? 0 : -1;
}

static int http_parser_url(const char *req, int *index, http_message_t *msg){
    AFD_STATE state = Q_URL;
    int start_index = *index;
    int i = *index;
    unsigned char c;

    while((c = (unsigned char)req[i]) != '\0' && state != Q_ERROR && state != Q_VERSION){
        switch (state)
        {
        case Q_URL:
            if(c == ' '){
                int url_len = i - start_index;

                if(url_len <= 0 || url_len >= HTTP_URL_MAX){
                    state = Q_ERROR;
                    break;
                }

                memcpy(msg->url, &req[start_index], (size_t)url_len);
                msg->url[url_len] = '\0';
                state = Q_VERSION;
                *index = i + 1;
            } else if(c >= 0x20 && c <= 0x7E){
                state = Q_URL;
            } else {
                state = Q_ERROR;
            }
            break;

        case Q_ERROR:
            break;

        default:
            state = Q_ERROR;
            break;
        }
        i++;
    }
    return (state == Q_VERSION) ? 0 : -1;
}

static int http_parser_version(const char *req, int *index, http_message_t *msg){
    AFD_STATE state = Q_VERSION;
    int start_index = *index;
    int i = *index;
    unsigned char c;

    while((c = (unsigned char)req[i]) != '\0' && state != Q_ERROR && state != Q_VERSION_END){
        switch (state)
        {
        case Q_VERSION:
            if(c == 'H'){
                state = Q_VERSION_T1;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_T1:
            if(c == 'T'){
                state = Q_VERSION_T2;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_T2:
            if(c == 'T'){
                state = Q_VERSION_P;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_P:
            if(c == 'P'){
                state = Q_VERSION_SLASH;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_SLASH:
            if(c == '/'){
                state = Q_VERSION_DIGIT_1;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_DIGIT_1:
            if(isdigit(c)){
                state = Q_VERSION_DOT;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_DOT:
            if(c == '.'){
                state = Q_VERSION_DIGIT_2;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_DIGIT_2:
            if(isdigit(c)){
                state = Q_VERSION_CR;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_CR:
            if(c == '\r'){
                state = Q_VERSION_LF;
            } else state = Q_ERROR;
            break;

        case Q_VERSION_LF:
            if(c == '\n'){
                state = Q_VERSION_END;
            } else state = Q_ERROR;
            break;

        default:
            state = Q_ERROR;
            break;
        }
        i++;
    }

    if(state == Q_VERSION_END){
        int version_len = (i - 2) - start_index; //exclui CR e LF

        if(version_len <= 0 || version_len >= HTTP_VERSION_MAX) return -1;

        memcpy(msg->version, &req[start_index], (size_t)version_len);
        msg->version[version_len] = '\0';
        *index = i;
        return 0;
    }
    return -1;
}

/* Caracteres válidos em nomes de header (tchar, RFC 7230). */
static int http_is_tchar(unsigned char c){
    if(isalnum(c)) return 1;
    return c != '\0' && strchr("!#$%&'*+-.^_`|~", c) != NULL;
}

/* Copia um par nome/valor delimitado por índices e o adiciona ao dicionário. */
static int http_parser_add_header(http_message_t *msg,
                                  const char *name, size_t name_len,
                                  const char *value, size_t value_len){
    char *name_copy;
    char *value_copy;
    int result;

    name_copy = malloc(name_len + 1);
    value_copy = malloc(value_len + 1);
    if(name_copy == NULL || value_copy == NULL){
        free(name_copy);
        free(value_copy);
        return -1;
    }

    memcpy(name_copy, name, name_len);
    name_copy[name_len] = '\0';
    memcpy(value_copy, value, value_len);
    value_copy[value_len] = '\0';

    result = http_headers_add(&msg->headers, name_copy, value_copy);

    free(name_copy);
    free(value_copy);
    return result;
}

static int http_parser_headers(const char *req, int *index, http_message_t *msg){
    int i = *index;

    while(req[i] != '\0'){
        int name_start;
        int name_len;
        int value_start;
        int value_len;

        /* Linha em branco: fim dos headers. */
        if(req[i] == '\r'){
            if(req[i + 1] != '\n') return -1;
            *index = i + 2; //Primeiro byte do body
            return 0;
        }
        if(req[i] == '\n') return -1; //CRLF obrigatório

        name_start = i;
        while(req[i] != '\0' && req[i] != ':'){
            if(!http_is_tchar((unsigned char)req[i])) return -1;
            i++;
        }
        if(req[i] != ':') return -1;

        name_len = i - name_start;
        if(name_len == 0) return -1;
        i++;

        /* Ignora espaços/tabs opcionais antes do valor. */
        while(req[i] == ' ' || req[i] == '\t') i++;

        value_start = i;
        while(req[i] != '\0' && req[i] != '\r'){
            if(req[i] == '\n') return -1; //LF sem CR
            i++;
        }
        if(req[i] != '\r' || req[i + 1] != '\n') return -1;

        value_len = i - value_start;
        /* Remove espaços/tabs ao final do valor. */
        while(value_len > 0 &&
              (req[value_start + value_len - 1] == ' ' ||
               req[value_start + value_len - 1] == '\t')){
            value_len--;
        }

        if(http_parser_add_header(msg, &req[name_start], (size_t)name_len,
                                  &req[value_start], (size_t)value_len) != 0){
            return -1;
        }

        i += 2; //Pula CRLF
    }

    return -1; //Terminou o buffer antes da linha em branco
}

static int http_parser_body(const char *req, int *index, http_message_t *msg){
    const char *content_length = http_headers_get(&msg->headers, "Content-Length");
    size_t available = strlen(&req[*index]);
    size_t body_len = available;

    if(content_length != NULL){
        char *end = NULL;
        unsigned long long declared = strtoull(content_length, &end, 10);

        if(end == content_length) return -1;

        while(*end == ' ' || *end == '\t') end++;
        if(*end != '\0') return -1;

        if(declared > (unsigned long long)available) return -1;
        body_len = (size_t)declared;
    }

    if(body_len == 0) return 0;
    return http_message_set_body(msg, &req[*index], body_len);
}

int http_parser(const char *req, http_message_t *msg){
    int index = 0;

    if(req == NULL || msg == NULL) return -1;

    http_message_init(msg);

    if(http_parser_method(req, &index, msg) != 0) goto error;
    if(http_parser_url(req, &index, msg) != 0) goto error;
    if(http_parser_version(req, &index, msg) != 0) goto error;
    if(http_parser_headers(req, &index, msg) != 0) goto error;
    if(http_parser_body(req, &index, msg) != 0) goto error;

    return 0;

error:
    http_message_free(msg);
    return -1;
}
