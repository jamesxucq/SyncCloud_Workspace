#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include"utility.h"
#include "qcgi_inte.h"
#include"invoke.h"

//

struct funivk *vtree_addi_children(struct funivk **pivk, char *line_txt) {
    struct funivk *aivk = (struct funivk *) malloc(sizeof (struct funivk));
    if (aivk) {
        memset(aivk, 0, sizeof (struct funivk));
        if (*pivk) {
            aivk->sibling = (*pivk)->children;
            (*pivk)->children = aivk;
        } else *pivk = aivk;
        strcpy(aivk->line_txt, line_txt);
    }
    // printf("create ivk:%x\n", aivk);
    return aivk;
}

struct funivk *find_vtree(struct funivk *fivk, char *line_txt) {
    struct funivk *aivk = NULL;
    //
    if (!fivk || !strcmp(line_txt, fivk->line_txt)) return fivk;
    aivk = find_vtree(fivk->children, line_txt);
    if (!aivk) aivk = find_vtree(fivk->sibling, line_txt);
    //
    return aivk;
}

unsigned int have_invoke_param(char *line_txt) {
    char *str;
    unsigned int fivk_type = FIVK_NULL;
    //
    for (str = line_txt; '\0' != *str; str++) {
        for (; ' ' == *str || '\t' == *str || '\r' == *str || '\n' == *str; str++);
        switch (*str) {
            case '"':
                for (++str; '"' != *str; str++) if ('\\' == *str) ++str;
                break;
            case '_':
                if (!strncmp("_req_", str, 0x05)) {
                    fivk_type |= FIVK_REQUIRE | FIVK_TPOOL;
                    str += 0x04;
                } else if (!strncmp("_sqlcon_", str, 0x08)) {
                    fivk_type |= FIVK_CONSQL;
                    str += 0x07;
                } else if (!strncmp("_tpool_", str, 0x07)) {
                    fivk_type |= FIVK_TPOOL;
                    str += 0x06;
                } else if (!strncmp("_param_", str, 0x07)) {
                    fivk_type |= FIVK_PARAM;
                    str += 0x06;
                }
                break;
        }
    }
    // printf("invoken:%x line_txt:%s\n", fivk_type, line_txt);
    return fivk_type;
}

unsigned int have_multi_param(char *line_txt) {
    char *str = strchr(line_txt, '(');
    if (!str) return FIVK_NULL;
    str = strchr(line_txt, ',');
    if (!str) return FIVK_NULL;
    return FIVK_CONBUFF;
}

unsigned int have_qcgi_invoke(char *line_txt) {
    int len;
    char qcgi_str[128];
    char *str;
    unsigned int fivk_type = FIVK_NULL;
    //
    for (str = line_txt; '\0' != *str; str++) {
        for (; ' ' == *str || '\t' == *str || '\r' == *str || '\n' == *str; str++);
        switch (*str) {
            case '"':
                for (++str; '"' != *str; str++) if ('\\' == *str) ++str;
                break;
            case '(':
                for (++str; ')' != *str; str++) if ('\\' == *str) ++str;
                break;
            default:
// printf("isalpha:|%s|", str);
                if (isalpha(*str)) {
                    char *se = strpbrk(str, " (");
                    if (se) {
                        len = se - str;
                        strzcpy(qcgi_str, str, len);
                    } else {
                        len = strlen(str);
                        strcpy(qcgi_str, str);
                    }
                    fivk_type |= check_qcig_invoke(qcgi_str);
                    str += len;
                }
                break;
        }
    }
    // printf("invoken:%x line_txt:%s\n", fivk_type, line_txt);
    return fivk_type;
}

struct funivk *parse_invoke(struct inte_line *plist) {
    struct funivk *fivk = NULL, *req_ivk = NULL;
    struct inte_line *iline;
    if (init_qcgi_hmap()) return NULL;
    //
    for (iline = plist; iline; iline = iline->next) {
        switch (STYPE_CODE(iline->type)) {
            case LINE_CTRL_TYPE:
                if (CTRL_REQUIRE_DONE & iline->type) {
                    vtree_addi_children(&fivk, iline->line_txt);
                    if (!req_ivk) req_ivk = fivk;
                }
                break;
            case LINE_FUNCTION_TYPE:
                fivk->invoke |= have_invoke_param(iline->line_txt);
                fivk->invoke |= have_multi_param(iline->line_txt);
                break;
            case LINE_EQUAL_TYPE:
                fivk->invoke |= have_invoke_param(iline->line_txt);
                if ((EQUAL_DECIMAL | EQUAL_UNDEFINE) & iline->type) fivk->invoke |= FIVK_CONBUFF;
                break;
            case LINE_CLANG_TYPE:
                if (CLANG_LINE & iline->type) {
                    fivk->invoke |= have_invoke_param(iline->line_txt);
                    fivk->invoke |= have_qcgi_invoke(iline->line_txt);
                } else if (CLANG_START & iline->type) fivk = find_vtree(req_ivk, iline->line_txt);
                else if (CLANG_END & iline->type) fivk = NULL;
                break;
        }
    }
    //
    fina_qcig_hmap();
    return req_ivk;
}

unsigned int recursive_invoke(struct funivk *fivk) {
    unsigned int fivk_type = FIVK_NULL;
    if (fivk) {
        fivk->invoke |= INHERIT_FIVK_TYPE(recursive_invoke(fivk->children));
        fivk_type = fivk->invoke;
        fivk_type |= recursive_invoke(fivk->sibling);
    }
    return fivk_type;
}

void print_invoke(struct funivk *fivk) {
    if (fivk) {
        printf("----------------------\n");
        printf("fivk:%08x invoke:%x children:%08x sibling:%08x \n%s\n", (unsigned int) fivk, fivk->invoke, (unsigned int) fivk->children, (unsigned int) fivk->sibling, fivk->line_txt);
        print_invoke(fivk->children);
        print_invoke(fivk->sibling);
    }
}

struct funivk *create_invoke(struct inte_line *plist) {
    struct funivk *fivk;
    fivk = parse_invoke(plist);
    recursive_invoke(fivk);
    // printf("----------- invoken -----------");
    // print_invoke(fivk);
    return fivk;
}

void destory_invoke(struct funivk *fivk) {
    if (fivk) {
        destory_invoke(fivk->children);
        destory_invoke(fivk->sibling);
        free(fivk);
    }
}
