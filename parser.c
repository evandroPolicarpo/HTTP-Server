#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"

int http_parser_method(const char *req, int *index, char *method){
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
                    strcpy(method, "CONNECT");
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
                    strcpy(method, "DELETE");
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
                    strcpy(method, "GET");
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
                    strcpy(method, "HEAD");
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
                    strcpy(method, "OPTIONS");
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
                    strcpy(method, "PATCH");
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
                    strcpy(method, "POST");
                }
                else state = Q_ERROR;
                break;

            //PUT Branch
            case Q_METHOD_PUT_2:
                if(c == 'T'){
                    state = Q_METHOD_END;
                    strcpy(method, "PUT");
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
                    strcpy(method, "QUERY");
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
                    strcpy(method, "TRACE");
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

int http_parser_url(const char *req, int *index, char *url){
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

                strncpy(url, &req[start_index], url_len);
                url[url_len] = '\0';
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

int http_parser_version(const char *req, int *index, char *version){
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

        strncpy(version, &req[start_index], version_len);
        version[version_len] = '\0';
        *index = i;
        return 0;
    }
    return -1;
}

int http_parser(const char *req){
    //...
    return 0;
}
