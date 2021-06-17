#include<string.h>

#include"common.h"
#include"line_type.h"

unsigned int get_control_type(char *ctrl_txt) {
    char *toke;
    unsigned int ctrl_type = 0;
    char coand[COAND_LENGTH];

    toke = strpbrk(ctrl_txt, "\t ()=");
    if (toke) {
        strncpy(coand, ctrl_txt, toke - ctrl_txt);
        coand[toke - ctrl_txt] = '\0';

        if (!strcasecmp(COAND_PAGE_SIZE, coand)) ctrl_type = CTRL_PAGE_SIZE;
        else if (!strcasecmp(COAND_AUTO_FLUSH, coand)) ctrl_type = CTRL_AUTO_FLUSH;
        else if (!strcasecmp(COAND_MIME_HEADER, coand)) ctrl_type = CTRL_MIME_HEADER;
        else if (!strcasecmp(COAND_INCLUDE, coand)) ctrl_type = CTRL_INCLUDE;
        else if (!strcasecmp(COAND_INCLUDE_ONCE, coand)) ctrl_type = CTRL_INCLUDE_ONCE;
        else if (!strcasecmp(COAND_REQUIRE, coand)) ctrl_type = CTRL_REQUIRE;
    }

    return ctrl_type;
}

unsigned int get_function_type(char *func_txt) {
    char *toke;
    unsigned int func_type = 0;
    char coand[COAND_LENGTH];

    toke = strpbrk(func_txt, "\t ()=");
    if (toke) {
        strncpy(coand, func_txt, toke - func_txt);
        coand[toke - func_txt] = '\0';

        if (!strcasecmp(COAND_ECHO, coand)) func_type = FUNC_ECHO;
        else if (!strcasecmp(COAND_PRINT, coand)) func_type = FUNC_PRINT;
    }

    return func_type;
}

unsigned int get_equal_type(char *equal_txt) {
    char *toke;
    unsigned int equal_type = 0;

    toke = equal_txt;
    if ('=' == *toke) {
        ++toke;
        if ('$' == *toke) equal_type = EQUAL_DECIMAL;
        else if ('%' == *toke) equal_type = EQUAL_UNDEFINE;
        else equal_type = EQUAL_STRING;
    }

    return equal_type;
}
