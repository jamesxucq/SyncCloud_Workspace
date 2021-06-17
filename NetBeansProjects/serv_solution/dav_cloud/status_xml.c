
#include "logger.h"
#include "status_xml.h"   

/*
<?xml version="1.0" encoding="UTF-8"?>
<status version="1.2.1">
<uid>12345321</uid>
<cached_anchor>xxxxxx</cached_anchor>
<location>xxxxxxxxx</location>
<session>xxxxxxxxx</session>
</status>
 */

#define START_HOME_LABEL        "<location>"
#define END_HOME_LABEL          "</location>"

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

int status_from_xmlfile(char *location, char *xml_txt) {
    char element_value[MAX_TEXT_LEN];
    char *parse_toke;
    //
    if (!xml_txt || !location) return -1; //modify by james 20110126    
    //
    parse_toke = xml_txt;
    GET_ELEMENT_VALUE(element_value, parse_toke, START_HOME_LABEL, END_HOME_LABEL);
    if (!parse_toke) return -1;
    strcpy(location, element_value);
    //
    return 0;
}


