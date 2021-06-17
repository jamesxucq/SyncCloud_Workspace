
#ifndef INTE_H
#define	INTE_H

#ifdef	__cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include"common.h"
#include"inte.h"
#include"line_type.h"

    struct clang_line {
        unsigned int type;
        char *line_txt;
        struct clang_line *next;
    };

    // html // comment

    struct inte_text {
        struct inte_line *next;
        unsigned int type;
        char text[TEXT_LENGTH];
    };

    // control
    // require pagesize autoflush mimeheader include
    // function
    // echo print
    // equal
    // = =$ =%

    struct inte_line {
        struct inte_line *next;
        unsigned int type;
        char line_txt[LINE_TXT];
    };

    /////////////////////////////////////////////
    struct inte_line *ilist_req(char *req_txt);
    struct inte_line *ilist_addlast(struct inte_line *ilist);
    struct inte_line *llist_addlast(struct inte_line *ilist);
    void destroy_ilist(struct inte_line *ilist);
    /////////////////////////////////////////////
    struct clang_line *clist_addlast(struct clang_line **clist);
    struct clang_line *clist_next_stion(struct clang_line *clist);
    void destroy_clist(struct clang_line *clist);
    /////////////////////////////////////////////
    void print_clist(struct clang_line *clist);
    void print_ilist(struct inte_line *ilist);

#ifdef	__cplusplus
}
#endif

#endif	/* INTE_H */

