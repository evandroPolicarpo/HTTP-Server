#ifndef PARSER_H
#define PARSER_H

#include "http.h"

/*
 * Parser de requisições HTTP.
 *
 * http_parser: ponto de entrada do parser da requisição completa. Inicializa
 *   `msg`, preenche os componentes (método, URL, versão, headers e body) e
 *   retorna 0 em caso de sucesso. Em caso de erro retorna -1 e libera os
 *   componentes já alocados em `msg`.
 */

/* Estados do autômato finito determinístico (AFD) usado no parsing. */
typedef enum {
    //MISC
    Q_METHOD,       //Entry Point
    Q_METHOD_END,   //Accept State
    Q_URL,          //URL State
    Q_VERSION,      //
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
    Q_METHOD_TRACE_4,

    //Version
    Q_VERSION_T1,
    Q_VERSION_T2,
    Q_VERSION_P,
    Q_VERSION_SLASH,
    Q_VERSION_DIGIT_1,
    Q_VERSION_DOT,
    Q_VERSION_DIGIT_2,
    Q_VERSION_CR, //Carriage Return
    Q_VERSION_LF, //Line Feed
    Q_VERSION_END, //Accept State (after LF)

    //Headers
    Q_HEADER,
    Q_HEADER_VALUE,
    Q_HEADER_LF,
    Q_HEADER_END

} AFD_STATE;

int http_parser(const char *req, http_message_t *msg);

#endif /* PARSER_H */
