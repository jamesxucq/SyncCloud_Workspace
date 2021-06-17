
#include "logger.h"
#include "query_xml.h"
#include "dav_stru.h"   

/*
<?xml version="1.0" encoding="UTF-8"?>
<query version="1.2.1">
<uid>2</uid>
<validation>VB48uIcNsS9JvOaY0btu6Kaqmo1wLGpY</validation>
<worker address="192.168.1.103" port="8090" />
<cached_anchor>0</cached_anchor>
<session_id>9209-1348697447-2</session_id>
</query>
*/

#define START_VALID_LABEL      "<validation>"
#define END_VALID_LABEL        "</validation>"
#define START_WORKER_LABEL      "<worker"
#define END_WORKER_LABEL        "/>"
#define START_CANCH_LABEL      "<cached_anchor>"
#define END_CANCH_LABEL        "</cached_anchor>"

#define GET_ELEMENT_VALUE(ELEMENT_VALUE, PARSE_TOKEN, LABEL_START, LABEL_END) { \
    char *elem_end, *elem_start; \
    if(elem_start = strstr(PARSE_TOKEN, LABEL_START)) { \
        elem_start += s_strlen(LABEL_START); \
        if (elem_end = strstr(elem_start, LABEL_END)) { \
            int length = elem_end - elem_start; \
            strncpy(ELEMENT_VALUE, elem_start, length); \
            ELEMENT_VALUE[length] = '\0'; \
            PARSE_TOKEN = elem_end + s_strlen(LABEL_END); \
        } else PARSE_TOKEN = NULL; \
    } else PARSE_TOKEN = NULL; \
}

/*  address="192.168.1.103" port="8090"  */
static void split_saddr(serv_addr *saddr, const char *addr_txt) {
    char *toksp = NULL, *tokep = NULL;
    //
    toksp = strchr(addr_txt, '"');
    if (!toksp) return;
    tokep = strchr(++toksp, '"');
    if (!tokep) return;
    tokep[0] = '\0';
    strcpy(saddr->sin_addr, toksp);
    //
    toksp = strchr(++tokep, '"');
    if (!toksp) return;
    tokep = strchr(++toksp, '"');
    if (!tokep) return;
    tokep[0] = '\0';
    saddr->sin_port = atoi(toksp);
}

int query_from_xmlfile(unsigned int *cached_anchor, serv_addr *saddr, char *req_valid, char *xml_txt) {
    char element_value[MAX_TEXT_LEN];
    char *parse_toke;
    //
    if (!xml_txt || !cached_anchor) return -1; //modify by james 20110126    
    //
    parse_toke = xml_txt;
    GET_ELEMENT_VALUE(element_value, parse_toke, START_VALID_LABEL, END_VALID_LABEL);
    if (!parse_toke) return -1;
    strcpy(req_valid, element_value);
    //
    GET_ELEMENT_VALUE(element_value, parse_toke, START_WORKER_LABEL, END_WORKER_LABEL);
    if (!parse_toke) return -1;
    split_saddr(saddr, element_value);
    //
    GET_ELEMENT_VALUE(element_value, parse_toke, START_CANCH_LABEL, END_CANCH_LABEL);
    if (!parse_toke) return -1;
    *cached_anchor = atoi(element_value);
    //
    return 0;
}


