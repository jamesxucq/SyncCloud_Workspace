/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef AUTH_REPLY_H
#define AUTH_REPLY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "sdtproto.h"
#include "dav_macro.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    enum STATUS_CODE {
        // Informational
        CONTINUE = 100, // 100 Continue 
        SWITCHINT_PROTOCOLS = 101, // 101 Switching Protocols
        PROCESSING = 102, // * 102 Processing // WebDAV
        // Successful
        OK = 200, // 200 OK
        CREATED = 201, // 201 Created
        ACCEPTED = 202, // 202 Accepted
        NON_AUTHORITATIVE_INFORMATION = 203, // 203 Non - Authoritative Information
        NO_CONTENT = 204, // 204 No Content
        RESET_CONTENT = 205, // 205 Reset Content
        PARTIAL_CONTENT = 206, // 206 Partial Content
        MULTI_STATUS = 207, // * 207 Multi - Status // WebDAV
        // Redirection
        MULTIPLE_CHOICES = 300, // 300 Multiple Choices
        MOVED_PERMANENTLY = 301, // 301 Moved Permanently
        FOUND = 302, // 302 Found
        SEE_OTHER = 303, // 303 See Other
        NOT_MODIFIED = 304, // 304 Not Modified
        USE_PROXY = 305, // 305 Use Proxy
        TEMPORARY_REDIRECT = 307, // 307 Temporary Redirect
        // Client Error
        BAD_REQUEST = 400, // 400 Bad Request
        UNAUTHORIZED = 401, // 401 Unauthorized
        FORBIDDEN = 403, // 403 Forbidden
        NOT_FOUND = 404, // 404 Not Found
        METHOD_NOT_ALLOWED = 405, // 405 Method Not Allowed
        NOT_ACCEPTABLE = 406, // 406 Not Acceptable
        PROXY_AUTHENTICATION_REQUIRED = 407, // 407 Proxy Authentication Required
        REQUEST_TIMEOUT = 408, // 408 Request Timeout
        CONFLICT = 409, // 409 Conflict
        GONE = 410, // 410 Gone
        LENGTH_REQUIRED = 411, // 411 Length Required
        PRECONDITION_FAILED = 412, // 412 Precondition Failed
        REQUEST_ENTITY_TOO_LARGE = 413, // 413 Request Entity Too Large
        REQUEST_URI_TOO_LOOG = 414, // 414 Request URI Too Long
        REQUESTED_RANGE_NOT_SATISFIABLE = 416, // 416 Requested Range Not Satisfiable
        EXPECTATION_FAILED = 417, // 417 Expectation Failed
        TOO_MANY_CONNECTIONS = 421, // * 421 There are too many connections from your internet address
        UNPROCESSABLE_ENTITY = 422, // * 422 Unprocessable Entity
        LOCKED = 423, // * 423 Locked
        FAILED_DEPENDENCY = 424, // * 424 Failed Dependency
        UNORDERED_COLLECTION = 425, // * 425 Unordered Collection
        UPGRADE_REQUIRED = 426, // * 426 Upgrade Required
        RETRY_WITH = 449, // * 449 Retry With
        // Server Error
        INTERNAL_SERVER_ERROR = 500, // 500 Internal Server Error
        NOT_IMPLEMENTED = 501, // 501 Not Implemented
        BAD_GATEWAY = 502, // 502 Bad Gateway
        SERVICE_UNAVAILABLE = 503, // 503 Service Unavailable
        GATEWAY_TIMEOUT = 504, // 504 Gateway Timeout
        HTTP_VERSION_NOT_SUPPORTED = 505, // 505 HTTP Version Not Supported
        VARIANT_ALSO_NEGOTIATES = 506, // * 506 Variant Also Negotiates
        INSUFFICIENT_STORAGE = 507, // * 507 Insufficient Storage // WebDAV
        BANDWIDTH_LIMIT_EXCEEDED = 509, // * 509 Bandwidth Limit Exceeded
        NOT_EXTENDED = 510, // * 510 Not Extended
    };    
    //   

    struct socks_request {
        char res_identity[MAX_PATH * 3];
	char auth_chks[MIN_TEXT_LEN];
        char req_valid[VALID_LENGTH];
        char params[MIN_TEXT_LEN];
    };

    struct socks_response {
        int status_code;
        //
        char content_length[DIGIT_LENGTH];
        char content_type[CONTENT_TYPE_LENGTH];
        char execute_status[EXECUTE_LENGTH];
    };

#define QUERY_REQUEST(REQ_VALUE, UID, AUTH_CHKS) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    strcpy(REQ_VALUE.auth_chks, AUTH_CHKS); \
}
    
#define STATUS_REQUEST(REQ_VALUE, UID, ADDR, PORT, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "addr=%s port=%d", ADDR, PORT); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}
    
//
    struct value_node {
        const char *text;
        int value;
    };
    
#ifndef TEXT_VALUE
#define TEXT_VALUE(VALUE, TABLE, TEXT) { \
    struct value_node *item; \
    for(item = (struct value_node *)TABLE; item->text; ++item) \
	if(!strcmp(item->text, TEXT)) \
            break; \
    VALUE = item->value; \
}
#endif

    static const struct value_node content_value_table[] = {
        {"text/xml", CONTENT_TYPE_XML},
        {"application/octet-stream", CONTENT_TYPE_OCTET},
        {"text/plain", CONTENT_TYPE_PLAIN},
        {NULL, 0}
    };
    
#define QUERY_RESPONSE(CONTENT_TYPE, CONTENT_LEN, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(CONTENT_TYPE, content_value_table, RES_VALUE.content_type) \
        CONTENT_LEN = atoi(RES_VALUE.content_length); \
    } \
}
    
#define STATUS_RESPONSE(CONTENT_TYPE, CONTENT_LEN, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(CONTENT_TYPE, content_value_table, RES_VALUE.content_type) \
        CONTENT_LEN = atoi(RES_VALUE.content_length); \
    } \
}
    
#define SET_ANCHOR_REQUEST(REQ_VALUE, UID, LAST_ANCHOR, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "anchor=%u", LAST_ANCHOR); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

    static const struct value_node execute_value_table[] = {
        {"successful", 0},
        {"exception", 1},
        {NULL, 0}
    };

#define SET_ANCHOR_RESPONSE(SET_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(SET_STATUS, execute_value_table, RES_VALUE.execute_status) \
    } \
}

    static const char *bool_text_table[] = {
        "false",
        "true"
    };

#define SET_LOCKED_REQUEST(REQ_VALUE, UID, SYNC_LOCKED, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    sprintf(REQ_VALUE.params, "locked=%s", bool_text_table[SYNC_LOCKED]); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

#define SET_LOCKED_RESPONSE(SET_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(SET_STATUS, execute_value_table, RES_VALUE.execute_status) \
    } \
}

#define KEEP_ALIVE_REQUEST(REQ_VALUE, UID, VALID) { \
    sprintf(REQ_VALUE.res_identity, "%u", UID); \
    strcpy(REQ_VALUE.req_valid, VALID); \
}

#define KEEP_ALIVE_RESPONSE(KEEP_STATUS, RES_VALUE) { \
    if (OK == RES_VALUE.status_code) { \
        TEXT_VALUE(KEEP_STATUS, execute_value_table, RES_VALUE.execute_status) \
    }\
}




#ifdef __cplusplus
}
#endif

#endif /* AUTH_REPLY_H */

