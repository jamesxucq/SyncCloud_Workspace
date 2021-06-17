#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "utility.h"
#include "builder.h"
#include "inte.h"
#include "invoke.h"

#define FUNCTION_DECLARE_SUFFIX(TEXT) TEXT = lscpy(TEXT, ";\n")
#define FUNCTION_HEATER_SUFFIX(TEXT) TEXT = lscpy(TEXT, " ")
#define CATSLINE(TEXT) TEXT = lscpy(TEXT, "\n")

//

static char *find_functi_decla(char *clcon, struct clang_line *clist, struct clang_line *rline) {
    struct clang_line *cline;
    // printf("cline->line_txt:|%s|", cline->line_txt);
    for (cline = clist; cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_START & cline->type)) {
            if (!check_decla_name(rline->line_txt, cline->line_txt)) {
                char *decla = strchr(cline->line_txt, '{');
                if (decla) {
                    clcon = lsncpy(clcon, cline->line_txt, decla - cline->line_txt);
                    FUNCTION_DECLARE_SUFFIX(clcon);
                }
            }
        }
    }
    return clcon;
}

//

static char *ctrl_page_decla(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline;
    for (cline = sline; eline != cline; cline = cline->next) {
        // printf("type:%08x line_txt:|%s|\n", cline->type, cline->line_txt);
        if ((LINE_CTRL_TYPE & cline->type) && ((CTRL_PAGE_SIZE | CTRL_AUTO_FLUSH | CTRL_INCLUDE | CTRL_INCLUDE_ONCE) & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CTRL_TYPE & cline->type) && (CTRL_REQUIRE_DONE & cline->type)) {
            // printf("rline->line_txt:|%s|", cline->line_txt);
            clcon = find_functi_decla(clcon, eline, cline);
        }
    }
    return clcon;
}

static char *appe_mime_head(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline;
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CTRL_TYPE & cline->type) && (CTRL_MIME_HEADER & cline->type)) {
            clcon = lscpy(clcon, cline->line_txt);
            break;
        }
    }
    return clcon;
}

static char *clang_page_extend(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline = NULL;
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_EXTEND & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type))
            break;
    }
    return clcon;
}

static char *clang_page_head(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline = NULL;
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_HEAD & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type))
            break;
    }
    return clcon;
}

static char *clang_page_bady(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline = NULL;
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_START & cline->type)) {
            continue;
        } else if ((LINE_CLANG_TYPE & cline->type) && (CLANG_BODY & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_COMMENT_TYPE | LINE_HTML_TYPE) & cline->type) clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CTRL_TYPE & cline->type) && (CTRL_REQUIRE_DONE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type))
            break;
    }
    return clcon;
}

static char *creat_page_clang(char *clcon, struct clang_line *sline, struct clang_line *eline, unsigned int req_ivk) {
    char line_txt[LINE_TXT];
    //
    clcon = lscpy(clcon, "#include <csp_header.h>\n");
    clcon = lscpy(clcon, "\n// define macro.\n");
    // output control command
    clcon = lscpy(clcon, "\n// control command declare.\n");
    if (FIVK_AFLUSH & req_ivk) clcon = lscpy(clcon, "#undef BUFFER_AUTO_FLUSH\n");
    clcon = ctrl_page_decla(clcon, sline, eline);
    CATSLINE(clcon);
    clcon = lscpy(clcon, "// main function extend declare.\n");
    clcon = clang_page_extend(clcon, sline, eline);
    CATSLINE(clcon);
    //
    clcon = lscpy(clcon, "\n// main function.\n");
    clcon = lscpy(clcon, "void *");
    clcon = lscpy(clcon, find_decal(line_txt, sline->line_txt));
    clcon = lscpy(clcon, "(MYSQL *_sqlcon_, tiny_pool_t *_tpool_) {\n");
    if (FIVK_CONBUFF & req_ivk) clcon = lscpy(clcon, "char _buffer_[PAGE_BUFFER_SIZE];\nmkzero(_buffer_);char *recon = _buffer_;\n");
    if (FIVK_REQUIRE & req_ivk) clcon = lscpy(clcon, "\n// parse queries.\nqentry_t _req_t_;\nqentry_init(&_req_t_, &recon);\nqentry_t *_req_ = &_req_t_;\n");
    if (FIVK_SESSION & req_ivk) clcon = lscpy(clcon, "\n// parse queries.\nqentry_t *_sess_ = qcgisess_init(&_req_, NULL);\n");
    clcon = lscpy(clcon, "\n// function head content.\n");
    clcon = clang_page_head(clcon, sline, eline);
    CATSLINE(clcon);
    if (!(FIVK_QPARSE & req_ivk) && (FIVK_REQUIRE & req_ivk)) clcon = lscpy(clcon, "\nqcgireq_parse(_req_, DEFAULT_QPARSE_METHOD, &recon);\n");
    clcon = lscpy(clcon, "\n// append control mime header.\n");
    if (FIVK_CONTYPE & req_ivk) clcon = lscpy(clcon, "#undef SET_CONTENT_TYPE\n");
    clcon = appe_mime_head(clcon, sline, eline);
    if (FIVK_CONBUFF & req_ivk) clcon = lscpy(clcon, "#ifdef SET_CONTENT_TYPE\nrecon = lscat(recon, DEFAULT_CONTENT_TYPE);\n#endif\n");
    clcon = lscpy(clcon, "\n// function bady content.\n");
    clcon = clang_page_bady(clcon, sline, eline); // output csp main function
    clcon = lscpy(clcon, "\n// end function bady.\n");
    CATSLINE(clcon);
    if (FIVK_CONBUFF & req_ivk) clcon = lscpy(clcon, "#ifndef BUFFER_AUTO_FLUSH\nfprintf(stdout, _buffer_);\nfflush(stdout);\n#endif\n");
    if (FIVK_SESSION & req_ivk) clcon = lscpy(clcon, "\n// free alloc session.\n_sess_->free(_sess_);\n");
    if (FIVK_REQUIRE & req_ivk) clcon = lscpy(clcon, "\n// free alloc queries.\n_req_t_.truncate(&_req_t_);\n");
    clcon = lscpy(clcon, "\nreturn (void *)((long long)-1);\n");
    clcon = lscpy(clcon, "}\n");
    CATSLINE(clcon);
    //
    return clcon;
}

static struct clang_line *build_page_clang(char *clcon, struct clang_line *clist, unsigned int req_ivk) {
    struct clang_line *sline, *cline;
    for (cline = clist; cline; cline = cline->next) {
        // printf("cline->type:%08X cline->line_txt:|%s|\n", cline->type, cline->line_txt);
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_START & cline->type)) sline = cline;
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type)) {
            cline = cline->next;
            creat_page_clang(clcon, sline, cline, req_ivk);

            break;
        }
    }
    return cline;
}

// require include incloue_once

static char *ctrl_obje_decla(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline;
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CTRL_TYPE & cline->type) && ((CTRL_INCLUDE | CTRL_INCLUDE_ONCE) & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CTRL_TYPE & cline->type) && (CTRL_REQUIRE_DONE & cline->type)) {

            clcon = find_functi_decla(clcon, eline, cline);
        }
    }
    return clcon;
}

static char *clang_obje_extend(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline;
    // if(LINE_HTML_TYPE & cline->type) printf("serv_clang1:|%s|\n", cline->line_txt);
    for (cline = sline; eline != cline; cline = cline->next) {

        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_EXTEND & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
    }
    return clcon;
}

static char *clang_obje_functi(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    struct clang_line *cline;
    //
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_START & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        break;
    }
    for (cline = sline; eline != cline; cline = cline->next)
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_HEAD & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_BODY & cline->type) && (CLANG_LINE & cline->type))
            clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_COMMENT_TYPE | LINE_HTML_TYPE) & cline->type) clcon = lscpy(clcon, cline->line_txt);
        else if ((LINE_CTRL_TYPE & cline->type) && (CTRL_REQUIRE_DONE & cline->type)) clcon = lscpy(clcon, cline->line_txt);
    }
    for (cline = sline; eline != cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type)) {
            clcon = lscpy(clcon, cline->line_txt);

            break;
        }
    }
    // if (LINE_CLANG_TYPE & cline->type) printf("clang|%s|", cline->line_txt);
    // if ((LINE_COMMENT_TYPE) & cline->type) printf("comm|%s|", cline->line_txt);
    return clcon;
}

static char *creat_obje_clang(char *clcon, struct clang_line *sline, struct clang_line *eline) {
    clcon = lscpy(clcon, "#include <csp_header.h>\n");
    clcon = ctrl_obje_decla(clcon, sline, eline);
    CATSLINE(clcon);
    clcon = clang_obje_extend(clcon, sline, eline);
    CATSLINE(clcon);
    clcon = clang_obje_functi(clcon, sline, eline);
    //
    return clcon;
}

static struct clang_line *build_obje_clang(char *clcon, struct clang_line *clist) {
    struct clang_line *sline, *cline;
    for (cline = clist; cline; cline = cline->next) {
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_START & cline->type)) sline = cline;
        if ((LINE_CLANG_TYPE & cline->type) && (CLANG_END & cline->type)) {
            cline = cline->next;
            creat_obje_clang(clcon, sline, cline);

            break;
        }
    }
    //
    return cline;
}

//
#define PASS_ROOT(CLIST)   CLIST = (CLIST)->next;

int build_clang_page(struct built_list *midd_list, struct clang_line *clist, unsigned int req_ivk) {
    char clcon[CLANG_TEXT_SIZE];
    char *clang;
    // printf("page clist type:%08X line_txt:|%s|\n", (clist->next)->type, (clist->next)->line_txt);
    PASS_ROOT(clist)
    clang = clist->line_txt;
    mkzero(clcon);
    struct clang_line *pline = build_page_clang(clcon, clist, req_ivk);
    if (!put_midd_file(midd_list, clang)) cfile_save(clang, (void*) clcon, strlen(clcon));
    while (pline) {
        clang = pline->line_txt;
        mkzero(clcon);
        // printf("pline type:%08X line_txt:|%s|\n", pline->type, pline->line_txt);
        pline = build_obje_clang(clcon, pline->next);

        if (!put_midd_file(midd_list, clang)) cfile_save(clang, (void*) clcon, strlen(clcon));
    }
    //
    return 0x00;
}

int build_clang_obje(struct built_list *midd_list, struct clang_line *clist) {
    char clcon[CLANG_TEXT_SIZE];
    char *clang;
    // printf("obje clist type:%08X line_txt:|%s|\n", (clist->next)->type, (clist->next)->line_txt);
    PASS_ROOT(clist)
    clang = clist->line_txt;
    mkzero(clcon);
    struct clang_line *pline = build_obje_clang(clcon, clist);
    if (!put_midd_file(midd_list, clang)) cfile_save(clang, (void*) clcon, strlen(clcon));
    while (pline) {
        clang = pline->line_txt;
        mkzero(clcon);
        // printf("pline type:%08X line_txt:|%s|\n", pline->type, pline->line_txt);
        pline = build_obje_clang(clcon, pline->next);
        if (!put_midd_file(midd_list, clang)) cfile_save(clang, (void*) clcon, strlen(clcon));
    }
    //
    return 0x00;
}