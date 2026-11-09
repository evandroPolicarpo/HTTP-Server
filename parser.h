#ifndef PARSER_H
#define PARSER_H

/*
 * Parser de requisições HTTP.
 *
 * http_parser_method: consome o método HTTP a partir de req[*index] e, em caso
 *   de sucesso, grava o método em `method` (ex.: "GET") e avança *index até o
 *   início da URL, retornando 0. Retorna -1 caso o método seja inválido.
 *   O buffer `method` deve ter espaço para o maior método ("OPTIONS") mais o
 *   terminador nulo, ou seja, no mínimo 8 bytes.
 *
 * http_parser: ponto de entrada do parser da requisição completa.
 */

/* Estados do autômato finito determinístico (AFD) usado no parsing. */
typedef enum {
    //MISC
    Q_METHOD,       //Entry Point
    Q_METHOD_END,   //Accept State
    Q_URL,          //URL State
    Q_ERROR,        //Refuse State

    //CONNECT Methods
    Q_METHOD_CONNECT_1,
    Q_METHOD_CONNECT_2,
    Q_METHOD_CONNECT_3,
    Q_METHOD_CONNECT_4,
    Q_METHOD_CONNECT_5,
    Q_METHOD_CONNECT_6,

    //DELETE Methods
    Q_METHOD_DELETE_1,
    Q_METHOD_DELETE_2,
    Q_METHOD_DELETE_3,
    Q_METHOD_DELETE_4,
    Q_METHOD_DELETE_5,

    //GET Methods
    Q_METHOD_GET_1,
    Q_METHOD_GET_2,

    //HEAD Methods
    Q_METHOD_HEAD_1,
    Q_METHOD_HEAD_2,
    Q_METHOD_HEAD_3,

    //OPTIONS Methods
    Q_METHOD_OPTIONS_1,
    Q_METHOD_OPTIONS_2,
    Q_METHOD_OPTIONS_3,
    Q_METHOD_OPTIONS_4,
    Q_METHOD_OPTIONS_5,
    Q_METHOD_OPTIONS_6,

    //P Methods (PATCH, POST or PUT)
    Q_METHOD_P_1,

    //PATCH Methods
    Q_METHOD_PATCH_2,
    Q_METHOD_PATCH_3,
    Q_METHOD_PATCH_4,

    //POST Methods
    Q_METHOD_POST_2,
    Q_METHOD_POST_3,

    //PUT Methods
    Q_METHOD_PUT_2,

    //QUERY Methods
    Q_METHOD_QUERY_1,
    Q_METHOD_QUERY_2,
    Q_METHOD_QUERY_3,
    Q_METHOD_QUERY_4,

    //TRACE Methods
    Q_METHOD_TRACE_1,
    Q_METHOD_TRACE_2,
    Q_METHOD_TRACE_3,
    Q_METHOD_TRACE_4
} AFD_STATE;

int http_parser_method(const char *req, int *index, char *method);
int http_parser(const char *req);

#endif /* PARSER_H */
