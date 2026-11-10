#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

/* Tamanhos máximos dos componentes de tamanho conhecido. */
#define HTTP_METHOD_MAX  8     /* "OPTIONS" + '\0' */
#define HTTP_URL_MAX     2048
#define HTTP_VERSION_MAX 16    /* "HTTP/1.1" + '\0' */

/*
 * Um header é um par chave/valor. As strings são alocadas dinamicamente,
 * pois tanto o nome quanto o valor vêm da requisição e têm tamanho variável.
 */
typedef struct http_header {
    char *name;
    char *value;
} http_header_t;

/*
 * Dicionário de headers. Como a quantidade de headers é desconhecida,
 * os itens ficam em um vetor dinâmico que cresce conforme necessário.
 */
typedef struct http_headers {
    http_header_t *items;
    size_t count;
    size_t capacity;
} http_headers_t;

/*
 * Componentes de uma mensagem HTTP.
 */
typedef struct http_message {
    char method[HTTP_METHOD_MAX];     /* ex.: "GET" */
    char version[HTTP_VERSION_MAX];   /* ex.: "HTTP/1.1" */
    char url[HTTP_URL_MAX];           /* ex.: "/index.html?q=1" */
    http_headers_t headers;           /* dicionário de headers */
    char *body;                       /* corpo (pode ser NULL) */
    size_t body_len;                  /* tamanho do corpo em bytes */
} http_message_t;

/* Inicializa a mensagem com os componentes vazios. */
void http_message_init(http_message_t *msg);

/* Libera toda a memória alocada para a mensagem. */
void http_message_free(http_message_t *msg);

/*
 * Adiciona um header ao dicionário, copiando nome e valor.
 * Retorna 0 em caso de sucesso e -1 em caso de erro de alocação.
 */
int http_headers_add(http_headers_t *headers, const char *name, const char *value);

/*
 * Busca o valor de um header pelo nome (comparação case-insensitive).
 * Retorna o valor ou NULL se o header não existir.
 */
const char *http_headers_get(const http_headers_t *headers, const char *name);

/* Remove um header pelo nome. Retorna 0 se removeu e -1 se não encontrou. */
int http_headers_remove(http_headers_t *headers, const char *name);

/* Libera todos os headers e zera o dicionário. */
void http_headers_free(http_headers_t *headers);

/*
 * Define o corpo da mensagem, copiando `len` bytes de `body`.
 * Retorna 0 em caso de sucesso e -1 em caso de erro de alocação.
 */
int http_message_set_body(http_message_t *msg, const char *body, size_t len);

#endif /* HTTP_H */
