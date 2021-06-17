#include<string.h>

#include "logger.h"
#include"utility.h"
#include"parser.h"

struct inte_line *create_clang_virtual(struct inte_line *list, char *vtual_txt, unsigned int vtual_type) {
    struct inte_line *item_vtual;
    item_vtual = ilist_addlast(list);
    item_vtual->type = LINE_CLANG_TYPE | vtual_type;
    if ((CLANG_CFILE | CLANG_START) & vtual_type) strcpy(item_vtual->line_txt, vtual_txt);
    return item_vtual;
}

struct inte_line *build_clang_start(struct inte_line *list, char *vtual_txt1, char *vtual_txt2) {
    struct inte_line *item_vtual;
    item_vtual = ilist_addlast(list);
    item_vtual->type = LINE_CLANG_TYPE | CLANG_CFILE;
    strcpy(item_vtual->line_txt, vtual_txt1);
    //
    item_vtual = ilist_addlast(list);
    item_vtual->type = LINE_CLANG_TYPE | CLANG_START;
    strcpy(item_vtual->line_txt, vtual_txt2);
    //
    return item_vtual;
}

struct inte_line *build_clang_end(struct inte_line *list) {
    struct inte_line *item_vtual;
    item_vtual = ilist_addlast(list);
    item_vtual->type = LINE_CLANG_TYPE | CLANG_END;
    return item_vtual;
}

struct inte_line *find_require_undo(struct inte_line *parse_list) {
    struct inte_line *ret_req = NULL, *require;
    for (require = parse_list; require; require = require->next) {
        if ((LINE_CTRL_TYPE & require->type) && (CTRL_REQUIRE & require->type)) {
            if (ret_req) {
                if (!strcmp(ret_req->line_txt, require->line_txt))
                    SET_REQUIRE_DONE(require)
                } else ret_req = require;
        }
    }
    ///////////////////////////////////////////////////
    return ret_req;
}

struct inte_text *hand_html_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_text *html;
    html = (struct inte_text *) llist_addlast(parse_list);
    html->type = LINE_HTML_TYPE;
    strcpy(html->text, line_txt);
    return html;
}

struct inte_text *hand_comment_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_text *comnt;
    comnt = (struct inte_text *) llist_addlast(parse_list);
    comnt->type = LINE_COMMENT_TYPE;
    strcpy(comnt->text, line_txt);
    return comnt;
}

struct inte_line *hand_control_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_line *ctrl;
    ctrl = ilist_addlast(parse_list);
    ctrl->type = LINE_CTRL_TYPE | get_control_type(line_txt);
    strcpy(ctrl->line_txt, line_txt);
    return ctrl;
}

struct inte_line *hand_function_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_line *func;
    func = ilist_addlast(parse_list);
    func->type = LINE_FUNCTION_TYPE | get_function_type(line_txt);
    strcpy(func->line_txt, line_txt);
    return func;
}

struct inte_line *hand_equal_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_line *equal;
    equal = ilist_addlast(parse_list);
    equal->type = LINE_EQUAL_TYPE | get_equal_type(line_txt);
    strcpy(equal->line_txt, line_txt);
    return equal;
}

struct inte_line *hand_clang_line(struct inte_line *parse_list, char *line_txt) {
    struct inte_line *clang;
    clang = ilist_addlast(parse_list);
    clang->type = LINE_CLANG_TYPE | CLANG_LINE;
    strcpy(clang->line_txt, line_txt);
    return clang;
}

int hand_clang_stion(struct inte_line *parse_list, char *stion_txt, unsigned int sattrib) {
    char *toke;
    unsigned int line_type;
    char line_txt[LINE_TXT];
    struct inte_line *line;

    toke = stion_txt;
    while ((toke = next_line(line_txt, &line_type, toke))) {
        // printf("line_type: %08X line_txt:|%s|\n", line_type, line_txt); // for test
        switch (line_type) {
            case LINE_COMMENT_TYPE:
                line = (struct inte_line *) hand_comment_line(parse_list, line_txt);
                line->type |= sattrib;
                break;
            case LINE_CTRL_TYPE:
                line = hand_control_line(parse_list, line_txt);
                line->type |= sattrib;
                break;
            case LINE_FUNCTION_TYPE:
                line = hand_function_line(parse_list, line_txt);
                line->type |= sattrib;
                break;
            case LINE_EQUAL_TYPE:
                line = hand_equal_line(parse_list, line_txt);
                line->type |= sattrib;
                break;
            case LINE_CLANG_TYPE:
                line = hand_clang_line(parse_list, line_txt);
                line->type |= sattrib;
                break;
        }
        // printf("line_type: %08X line_txt:|%s|\n", line->type, line->line_txt); 
    }
    //
    return 0x00;
}

//

static char *_strtok_(int *toke_type, char *stri) {
    char *sa = strstr(stri, CLANG_START_TOKEN);
    char *sb = strstr(stri, CLANG_END_TOKEN);
    if (sa && sb) {
        if (sa < sb) {
            *toke_type = 0x01;
            return sa;
        } else {
            *toke_type = -1;
            return sb;
        }
    }
    if (sa) {
        *toke_type = 0x01;
        return sa;
    }
    if (sb) {
        *toke_type = -1;
        return sb;
    }
    //
    *toke_type = 0x00;
    return NULL;
}

// 0x00:ok -1/0x01:err

char *check_cspla_toke(char *parse_text) {
    char *parse_toke;
    int toke_type;
    int toke_numb = 0x00;
    // printf("------------------------------------------------\n%s\n", parse_text); // for test
    if (!parse_text || '\0' == parse_text[0]) return NULL;
    ////////////////////////////////////////////////////////
    parse_toke = _strtok_(&toke_type, parse_text);
    if (!parse_toke) return 0x00;
    toke_numb += toke_type;
    while ((parse_toke = _strtok_(&toke_type, parse_toke + CLANG_TOKLEN))) {
        toke_numb += toke_type;
        if (0x01 < toke_numb) break;
        else if (-1 > toke_numb) break;
    }
    ////////////////////////////////////////////////////////
    return parse_toke;
}

int parse_cspla_text(struct inte_line *parse_list, char *cspla_text) {
    char *parse_toke;
    unsigned int stion_type;
    char stion_txt[SECTION_LENGTH];
    // printf("cspla_data:|%s|\n", cspla_data);
    parse_toke = cspla_text;
    while ((parse_toke = search_section(stion_txt, &stion_type, parse_toke))) {
        int chkval = check_valid(stion_txt);
        if (-1 == chkval) return -1;
        else if (0x01 == chkval) continue;
        // printf("stion_txt:|%s|\n", stion_txt); // for test
        switch (STYPE_CODE(stion_type)) {
            case SECTION_HTML_TYPE:
                hand_html_line(parse_list, stion_txt);
                // printf("html line_txt:|%s|\n", stion_txt); 
                break;
            case SECTION_CLANG_TYPE:
                hand_clang_stion(parse_list, stion_txt, SATTR_CODE(stion_type));
                break;
        }
    }
    //
    return 0x00;
}

int parse_cspla_data(struct inte_line *parse_list, char *cspla_data) {
    char *parse_toke = check_cspla_toke(cspla_data);
    if (parse_toke) {
        _LOG_FATAL("cspla token error.");
        display_lineno(cspla_data, parse_toke);
        return -1;
    }
    if (parse_cspla_text(parse_list, cspla_data)) return -1;
    return 0x00;
}