
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "common.h"
#include "logger.h"
#include "utility.h"
#include "line_type.h"
#include "inte.h"
#include "parser.h"
#include "builder.h"
#include "compile.h"
//

int recursive_parse(struct inte_line *parse_list, char *cspla) {
    char req_file[FILE_NAME_LEN];
    int parse_ok = 0x00;
    struct inte_line *require;

    // printf("++++++++++++++++\n"); // for test
    while ((require = find_require_undo(parse_list)) && !parse_ok) {
        char *cspla_data = cfile_load(ctrl_require_file(req_file, require->line_txt), NULL);
// printf("req_file:%s require->line_txt:%s\n", req_file, require->line_txt);
        if (!cspla_data) {
            _LOG_FATAL("can not load file. %s", req_file);
            parse_ok = 0x01;
            break;
        }
        // _LOG_INFO("parsing->%s", req_file);
        _LOG_PARSE_SET(NULL, req_file);
        build_clang_start(parse_list, req_file, require->line_txt);
        parse_ok = parse_cspla_data(parse_list, cspla_data);
        build_clang_end(parse_list);
        SET_REQUIRE_DONE(require)
        free(cspla_data);
        ///////////////////////////////////
        _LOG_PARSE_SET(NULL, cspla);
    }
    //
    return parse_ok;
}

int csp_compile_page(struct built_list *midd_list, char *cspla, int function) {
    struct inte_line *plist = ilist_req(cspla);
    if (recursive_parse(plist, cspla)) {
        destroy_ilist(plist);
        return -1;
    }
    _LOG_PARSE_SET(NULL, cspla);
    /////////////////////////////////////
    // print_ilist(plist);
    struct clang_line *clist = NULL;
    unsigned int root_invoke = conv_cspla_clang(&clist, plist);
    destroy_ilist(plist);
    // print_clist(clist);
    if (function) build_clang_obje(midd_list, clist);
    else build_clang_page(midd_list, clist, root_invoke);
    // printf("file size:%d compile:|%s|\n", strlen(clcon), clcon);
    destroy_clist(clist);
    /////////////////////////////////////
    return 0x00;
}

/* * */
int cspla_to_clang(struct built_list *midd_list, char *cspla, int function) {
    _LOG_PARSE_SET(NULL, cspla);
    if (is_cspla(cspla)) {
        _LOG_ERROR("input file is not cspla!");
        return -1;
    }
    if (access(cspla, F_OK)) {
        _LOG_ERROR("no such file or directory");
        return -1;
    }
    // if (!csp_compile(midd_list, cspla)) printf("compile %s to %s\n", cspla, clang);
    if (csp_compile_page(midd_list, cspla, function)) return -1;
    return 0x00;
}

//

int handle_compile(struct built_list *midd_list, struct spc_param *spc_para) {
    int inde;
    for (inde = 0x00; spc_para->cspa_file[inde]; inde++) {
        if (cspla_to_clang(midd_list, spc_para->cspa_file[inde], spc_para->function))
            break;
    }
    //
    return 0x00;
}