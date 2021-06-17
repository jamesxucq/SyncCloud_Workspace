#include<string.h>

#include"utility.h"
#include"invoke.h"
#include"conv.h"

//

#define ECHO_ARGU(lstr, pstr, ftype, invok) \
    if (ftype & invok) { \
        lstr = lscpy(lstr, pstr); \
        if (ftype < (~(FIVK_CONTYPE|FIVK_AFLUSH|FIVK_QPARSE|ftype) & invok)) \
            lstr = lscpy(lstr, ", "); \
    }
//
#define FUNCTION_INVOKE(line_str, ctrl_str, ctrl_str1, ctrl, fivk) { \
    ctrl_require(ctrl_str, ctrl_str1, ctrl->line_txt); \
    struct funivk *invk = find_vtree(fivk, ctrl->line_txt); \
    line_str = lsprif(line_str, "if(!(recon = %s(", ctrl_str); \
    ECHO_ARGU(line_str, "recon", FIVK_CONBUFF, invk->invoke) \
    ECHO_ARGU(line_str, "_req_", FIVK_REQUIRE, invk->invoke) \
    ECHO_ARGU(line_str, "_sqlcon_", FIVK_CONSQL, invk->invoke) \
    ECHO_ARGU(line_str, "_tpool_", FIVK_TPOOL, invk->invoke) \
    ECHO_ARGU(line_str, "_sess_", FIVK_SESSION, invk->invoke) \
    if (FIVK_PARAM & invk->invoke) line_str = lscpy(line_str, "(void *)"); \
    ECHO_ARGU(line_str, ctrl_str1, FIVK_PARAM, invk->invoke) \
    line_str = lscpy(line_str, "))) return NULL;\n"); \
}

void conv_control(struct clang_line *cline, struct inte_line *iline, struct funivk *fivk) {
    char ctrl_str[STRING_LENGTH], ctrl_str1[STRING_LENGTH];
    char line_txt[LINE_TXT];
    char *last_char;
    //
    switch (ENTRY_CODE(iline->type)) {
        case CTRL_PAGE_SIZE:
            ctrl_page_size(ctrl_str, iline->line_txt);
            sprintf(line_txt, "#undef PAGE_BUFFER_SIZE\n#define PAGE_BUFFER_SIZE %s\n", ctrl_str);
            break;
        case CTRL_AUTO_FLUSH:
            if (ctrl_auto_flush(iline->line_txt)) sprintf(line_txt, "#undef BUFFER_AUTO_FLUSH\n");
            break;
        case CTRL_MIME_HEADER:
            if (ctrl_mime_header(ctrl_str, iline->line_txt)) {
                last_char = lsprif(line_txt, "recon = lscpy(recon, \"%s\");\n", ctrl_str);
                strcpy(last_char, "#undef SET_CONTENT_TYPE\n");
            } else sprintf(line_txt, "recon = lscpy(recon, \"%s\");\n", ctrl_str);
            break;
        case CTRL_INCLUDE:
            ctrl_include(ctrl_str, iline->line_txt);
            sprintf(line_txt, "#include %s\n", ctrl_str);
            break;
        case CTRL_INCLUDE_ONCE:
            ctrl_include_once(ctrl_str, ctrl_str1, iline->line_txt);
            sprintf(line_txt, "#ifndef %s\n#define %s\n#include %s\n#endif\n", ctrl_str1, ctrl_str1, ctrl_str);
            break;
        case CTRL_REQUIRE_DONE:
            last_char = line_txt;
            FUNCTION_INVOKE(last_char, ctrl_str, ctrl_str1, iline, fivk)
            break;
    }
    //
    cline->type = iline->type;
    cline->line_txt = strdup(line_txt);
}

void conv_function(struct clang_line *cline, struct inte_line *iline) {
    char func_str[STRING_LENGTH], func_str1[STRING_LENGTH];
    char line_txt[LINE_TXT];
    // printf("inline type:%08X line_txt:|%s|\n", iline->type, iline->line_txt);
    switch (ENTRY_CODE(iline->type)) {
        case FUNC_ECHO:
            func_echo(func_str, func_str1, iline->line_txt);
            if ('\0' == func_str1[0]) sprintf(line_txt, "recon = lscpy(recon, %s);\n", func_str);
            else sprintf(line_txt, "recon = lsprif(recon, %s%s);\n", func_str, func_str1);
            break;
        case FUNC_PRINT:
            func_print(func_str, func_str1, iline->line_txt);
            if ('\0' == func_str1[0]) sprintf(line_txt, "recon = lscpy(recon, %s);\n", func_str);
            else sprintf(line_txt, "recon = lsprif(recon, %s%s);\n", func_str, func_str1);
            break;
    }
    cline->type = LINE_CLANG_TYPE | SATTR_CODE(iline->type) | CLANG_LINE;
    cline->line_txt = strdup(line_txt);
    // printf("cline type:%08X line_txt:|%s|\n", cline->type, cline->line_txt);
}

void conv_equal(struct clang_line *cline, struct inte_line *iline) {
    char equal_str[STRING_LENGTH], equal_str1[STRING_LENGTH];
    char line_txt[LINE_TXT];
    //
    switch (ENTRY_CODE(iline->type)) {
        case EQUAL_STRING:
            equal_string(equal_str, iline->line_txt);
            sprintf(line_txt, "recon = lscpy(recon, %s);\n", equal_str);
            // cline->type = LINE_CLANG_TYPE | CLANG_LINE;
            // printf("+++ %s", line_txt);
            break;
        case EQUAL_DECIMAL:
            equal_decimal(equal_str, iline->line_txt);
            sprintf(line_txt, "recon = lsprif(recon, \"%%d\", %s);\n", equal_str);
            // cline->type = LINE_CLANG_TYPE | CLANG_LINE | CLANG_LBUFF;
            break;
        case EQUAL_UNDEFINE:
            equal_undefine(equal_str, equal_str1, iline->line_txt);
            sprintf(line_txt, "recon = lsprif(recon, \"%s\", %s);\n", equal_str, equal_str1);
            // cline->type = LINE_CLANG_TYPE | CLANG_LINE | CLANG_LBUFF;
            break;
    }
    cline->type = LINE_CLANG_TYPE | SATTR_CODE(iline->type) | CLANG_LINE;
    cline->line_txt = strdup(line_txt);
}

void conv_comment(struct clang_line *cline, struct inte_text *comme) {
    char comen_line[TEXT_LENGTH];
    sprintf(comen_line, "//%s", comme->text + 0x01);
    cline->type = comme->type;
    cline->line_txt = strdup(comen_line);
}

void conv_html(struct clang_line *cline, struct inte_text *html) {
    char html_line[TEXT_LENGTH];
    repl_str(html->text);
    // printf("conv_html:|%s|\n", html->text);
    sprintf(html_line, "recon = lscpy(recon, \"%s\");\n", html->text);
    cline->type = html->type;
    cline->line_txt = strdup(html_line);
}

//
#define FUNCTION_START_DECLARE(line_str, lang_str, clang, fivk) { \
    ctrl_require(lang_str, NULL, clang->line_txt); \
    struct funivk *invk = find_vtree(fivk, clang->line_txt); \
    invoke = invk->invoke; \
    line_str = lsprif(line_str, "char *%s(", lang_str); \
    ECHO_ARGU(line_str, "char *recon", FIVK_CONBUFF, invoke) \
    ECHO_ARGU(line_str, "qentry_t *_req_", FIVK_REQUIRE, invoke) \
    ECHO_ARGU(line_str, "MYSQL *_sqlcon_", FIVK_CONSQL, invoke) \
    ECHO_ARGU(line_str, "tiny_pool_t *_tpool_", FIVK_TPOOL, invoke) \
    ECHO_ARGU(line_str, "qentry_t *_sess_", FIVK_SESSION, invoke) \
    ECHO_ARGU(line_str, "(void *)_param_", FIVK_PARAM, invoke) \
    line_str = lscpy(line_str, ") {\n"); \
}

void conv_clang(struct clang_line *cline, struct inte_line *iline, struct funivk *fivk) {
    char lang_str[STRING_LENGTH];
    char line_txt[LINE_TXT];
    char *last_char;
    static unsigned int invoke;
    //
    switch (ENTRY_CODE(iline->type)) {
        case CLANG_CFILE:
            clang_path(line_txt, iline->line_txt);
            cline->line_txt = strdup(line_txt);
            break;
        case CLANG_START:
            last_char = line_txt;
            FUNCTION_START_DECLARE(last_char, lang_str, iline, fivk)
            cline->line_txt = strdup(line_txt);
            break;
        case CLANG_LINE:
            cline->line_txt = strdup(iline->line_txt);
            break;
        case CLANG_END:
            if(FIVK_CONBUFF & invoke) strcpy(line_txt, "\n\nreturn recon;\n}\n\n");
            else strcpy(line_txt, "\n\nreturn NULL;\n}\n\n");
            cline->line_txt = strdup(line_txt);
            break;
    }
    cline->type = iline->type;
}

int conv_to_clang(struct clang_line **clang_list, struct inte_line *parse_list, struct funivk *fivk) {
    struct clang_line *cline;
    struct inte_line *iline;
    //
    for (iline = parse_list; iline; iline = iline->next) {
        cline = clist_addlast(clang_list);
        // printf("inline type:%08X line_txt:|%s|\n", iline->type, iline->line_txt);
        ////////////////////////////////////////
        switch (STYPE_CODE(iline->type)) {
            case LINE_COMMENT_TYPE:
                conv_comment(cline, (struct inte_text *) iline);
                break;
            case LINE_HTML_TYPE:
                // printf("1 OOOOOOOOOO\n");
                conv_html(cline, (struct inte_text *) iline);
                break;
            case LINE_CTRL_TYPE:
                // printf("2 OOOOOOOOOO\n");
                conv_control(cline, iline, fivk);
                break;
            case LINE_FUNCTION_TYPE:
                // printf("3 OOOOOOOOOO\n");
                conv_function(cline, iline);
                break;
            case LINE_EQUAL_TYPE:
                // printf("4 OOOOOOOOOO\n");
                conv_equal(cline, iline);
                break;
            case LINE_CLANG_TYPE:
                // printf("5 OOOOOOOOOO\n");
                conv_clang(cline, iline, fivk);
                break;
        }
        // printf("cline type:%08X line_txt:|%s|\n", cline->type, cline->line_txt);
    }
    ///////////////////////////////////////////////////
    return 0;
}

unsigned int conv_cspla_clang(struct clang_line **clang_list, struct inte_line *parse_list) {
    struct funivk *fivk;
    unsigned int req_invoke;
    //
    fivk = create_invoke(parse_list);
    if (!fivk) {
        printf("spc: fatal error!\n");
        return 0x00;
    }
    conv_to_clang(clang_list, parse_list, fivk);
    req_invoke = fivk->invoke;
    destory_invoke(fivk);
    //
    return req_invoke;
}