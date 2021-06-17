
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"inte.h"

struct inte_line *ilist_req(char *req_txt) {
    struct inte_line *item_req;
    item_req = (struct inte_line *) malloc(sizeof (struct inte_line));
    item_req->next = NULL;
    item_req->type = LINE_CTRL_TYPE | CTRL_REQUIRE;
    sprintf(item_req->line_txt, "%s \"%s\"", COAND_REQUIRE, req_txt);
    return item_req;
}

struct inte_line *ilist_addlast(struct inte_line *ilist) {
    struct inte_line *laline = ilist;
    struct inte_line *aline = (struct inte_line *) malloc(sizeof (struct inte_line));
    if (aline) {
        aline->next = NULL;
        while (laline->next) laline = laline->next;
        laline->next = aline;
    }
    return aline;
}

struct inte_line *llist_addlast(struct inte_line *ilist) {
    struct inte_line *laline = ilist;
    struct inte_line *aline = (struct inte_line *) malloc(sizeof (struct inte_text));
    if (aline) {
        aline->next = NULL;
        while (laline->next) laline = laline->next;
        laline->next = aline;
    }
    return aline;
}

void destroy_ilist(struct inte_line *ilist) {
    struct inte_line *iline, *aline;
    for (iline = ilist; iline; iline = aline) {
        aline = iline->next;
        free(iline);
    }
    ///////////////////////////////////////////////////
}

struct clang_line *clist_addlast(struct clang_line **clist) {
    struct clang_line *laline;
    struct clang_line *aline = (struct clang_line *) malloc(sizeof (struct clang_line));
    if (aline) {
        aline->next = NULL;
        if (*clist) {
            laline = *clist;
            while (laline->next) laline = laline->next;
            laline->next = aline;
        } else *clist = aline;
    }
    return aline;
}

struct clang_line *clist_next_stion(struct clang_line *clist) {
    struct clang_line *cline;
    for (cline = clist; cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type)) 
            break;
    }
    return cline->next;
}

void destroy_clist(struct clang_line *clist) {
    struct clang_line *cline, *aline;
    for (cline = clist; cline; cline = aline) {
        aline = cline->next;
        if (cline->line_txt) free(cline->line_txt);
        free(cline);
    }
    ///////////////////////////////////////////////////
}

void print_clist(struct clang_line *clist) {
    struct clang_line *cline;
// printf("---------------------- clist ----------------------\n");
    for (cline = clist; cline; cline = cline->next) {
// printf("----------------------\n");
// printf("cline:%x line_txt:%x type:%x\n%s\n", cline, cline->line_txt, cline->type, cline->line_txt);
    }
    ///////////////////////////////////////////////////
}

void print_ilist(struct inte_line *ilist) {
    struct inte_line *iline;
// printf("---------------------- ilist ----------------------\n");
    for (iline = ilist; iline; iline = iline->next) {
// printf("----------------------\n");
// printf("iline:%x text:%x type:%x\n%s\n", iline, iline->line_txt, iline->type, iline->line_txt);
    }
    ///////////////////////////////////////////////////
}