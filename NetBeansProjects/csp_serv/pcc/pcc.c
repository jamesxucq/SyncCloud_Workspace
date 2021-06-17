/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "utili.h"

//
#define MAX_PARAM_CNT   256
// #define MAX_PATH 260
// #define MAX_PARAM_LEN 32
#define SPC_COMPI_TOK     "spc"
#define GPP_COMPI_TOK     "g++"
#define ARCHIVE_TOK       "ar"

#define PCC_CACHE_FILE ".pcc_cache"

//
#define INVA_FILE_TYPE      -1
#define CSPA_FILE_TYPE      0x0001
#define CLAN_FILE_TYPE      0x0003
#define OBJ_FILE_TYPE       0x0005
#define LIB_FILE_TYPE       0x0007
#define COMPI_PARAM_TYPE    0x0009

struct pcc_param {
    char *cspa_file[MAX_PARAM_CNT];
    char *clan_file[MAX_PARAM_CNT];
    char *obj_file[MAX_PARAM_CNT * 8];
    char *lib_file[MAX_PARAM_CNT];
    char *comp_param[MAX_PARAM_CNT];
};

/*
 */
// -1:error 0:exception 1:csp

int param_type(char *str) {
    if ('-' == str[0x00]) return COMPI_PARAM_TYPE;
    char *suffix = strrchr(str, '.');
    if (!suffix) return COMPI_PARAM_TYPE;
    else if (!strcasecmp(suffix, ".csp")) return CSPA_FILE_TYPE;
    else if (!strcasecmp(suffix, ".cpp")) return CLAN_FILE_TYPE;
    else if (!strcasecmp(suffix, ".o")) return OBJ_FILE_TYPE;
    else if (!strcasecmp(suffix, ".a")) return LIB_FILE_TYPE;
    else if (!strcasecmp(suffix, ".so")) return COMPI_PARAM_TYPE;
    //
    return INVA_FILE_TYPE;
}

int split_argum(char *comp_param[], int split_type, int numbe, char *param[]) {
    int param_inde = 0x00;
    //
    int inde;
    for (inde = 0x00; numbe > inde; inde++) {
        if (split_type == param_type(param[inde])) {
            // printf("comp_param:%s\n", param[inde]);
            comp_param[param_inde] = strdup(param[inde]);
            param_inde++;
        }
    }
    comp_param[param_inde] = NULL;
    //
    return param_inde;
}

//

void list_printf(char *label, char *param[]) {
    printf("--------- ");
    printf(label);
    printf(" ---------\n");
    //
    int inde;
    for (inde = 0x00; param[inde]; inde++)
        printf("%s ", param[inde]);
    printf("\n");
}

//

int handle_param(struct pcc_param *pcc_para, int numbe, char *param[]) {
    int have_file = 0x00;
    int param_count;
    //
    param_count = split_argum(pcc_para->cspa_file, CSPA_FILE_TYPE, numbe, param);
    have_file |= param_count;
    if (MAX_PARAM_CNT < param_count) {
        printf("sorry, csp page is too much!\n");
        return -1;
    }
    list_printf("cspa_file", pcc_para->cspa_file);
    if (pcc_file_exis(pcc_para->cspa_file)) return -1;
    //
    param_count = split_argum(pcc_para->clan_file, CLAN_FILE_TYPE, numbe, param);
    have_file |= param_count;
    if (MAX_PARAM_CNT < param_count) {
        printf("sorry, clang page is too much!\n");
        return -1;
    }
    list_printf("clan_file", pcc_para->clan_file);
    if (pcc_file_exis(pcc_para->clan_file)) return -1;
    //
    param_count = split_argum(pcc_para->obj_file, OBJ_FILE_TYPE, numbe, param);
    have_file |= param_count;
    if (MAX_PARAM_CNT < param_count) {
        printf("sorry, obj page is too much!\n");
        return -1;
    }
    list_printf("obj_file", pcc_para->obj_file);
    if (pcc_file_exis(pcc_para->obj_file)) return -1;
    //
    param_count = split_argum(pcc_para->lib_file, LIB_FILE_TYPE, numbe, param);
    have_file |= param_count;
    if (MAX_PARAM_CNT < param_count) {
        printf("sorry, lib page is too much!\n");
        return -1;
    }
    list_printf("lib_file", pcc_para->lib_file);
    if (pcc_file_exis(pcc_para->lib_file)) return -1;
    //
    param_count = split_argum(pcc_para->comp_param, COMPI_PARAM_TYPE, numbe, param);
    if (MAX_PARAM_CNT < param_count) {
        printf("sorry, compile param is too much!\n");
        return -1;
    }
    list_printf("comp_param", pcc_para->comp_param);
    //
    if (0x00 == have_file) {
        printf("no input file!\n");
        return -1;
    }
    //
    return 0x00;
}

//

int extract_lib(char *middle_file, char *lib_file) {
    FILE *cache_fp = freopen64(middle_file, "a", stdout);
    //
    char *extra_line[4] = {ARCHIVE_TOK, "-t", lib_file, NULL};
    pid_t pid = vfork();
    if (0x00 > pid) {
        fclose(cache_fp);
        freopen64("/dev/tty", "w", stdout);
        printf("fork art error!\n");
        return -1;
    } else if (!pid) {
        // list_printf("art extract lib.", extra_line);
        execvp(extra_line[0x00], extra_line);
        printf("art extract error.\n");
        exit(-1);
    }
    wait(NULL);
    //
    fclose(cache_fp);
    freopen64("/dev/tty", "w", stdout);
    //
    extra_line[0x01] = "-x";
    pid = vfork();
    if (0x00 > pid) {
        printf("fork arx error!\n");
        return -1;
    } else if (!pid) {
        list_printf("arx extract lib.", extra_line);
        execvp(extra_line[0x00], extra_line);
        printf("arx extract error.\n");
        exit(-1);
    }
    wait(NULL);
    //
    return 0x00;
}

#define BUILD_PACKAGE_PARAM(LINE_TXT, COMPI_TOK, PARAM, OBJ_FILE) \
    int pind, lind; \
    LINE_TXT[0x00] = COMPI_TOK; \
    LINE_TXT[0x01] = "-shared"; LINE_TXT[0x02] = "-fPIC"; \
    for(pind=0x03, lind=0x00; PARAM[lind]; lind++) { \
        if(!strncmp("-o", PARAM[lind], 0x02)) { \
            LINE_TXT[pind++] = PARAM[lind++]; \
            LINE_TXT[pind++] = PARAM[lind]; \
        } \
    } \
    for(lind=0x00; OBJ_FILE[lind]; pind++, lind++) \
        LINE_TXT[pind] = OBJ_FILE[lind]; \
    LINE_TXT[pind] = NULL;

int package_lib(char *comp_param[], char *obj_file[]) {
    char *parcka_line[MAX_PARAM_CNT * 8];
    //
    BUILD_PACKAGE_PARAM(parcka_line, GPP_COMPI_TOK, comp_param, obj_file)
    pid_t pid = vfork();
    if (0x00 > pid) {
        printf("fork gcc error!\n");
        return -1;
    } else if (!pid) {
        list_printf("gcc package", parcka_line);
        execvp(parcka_line[0x00], parcka_line);
        printf("gcc compile error.\n");
        exit(-1);
    }
    wait(NULL);
    //
    return 0x00;
}

//

int csp_compile(char *middle_file, char *cspa_file) {
    char *para_line[5] = {SPC_COMPI_TOK, "-s", middle_file, cspa_file, NULL};
    //
    pid_t pid = vfork();
    if (0x00 > pid) {
        printf("fork spc error!\n");
        return -1;
    } else if (!pid) {
        list_printf("spc compiling", para_line);
        execvp(para_line[0x00], para_line);
        printf("spc compile error.\n");
        exit(-1);
    }
    wait(NULL);
    //
    return 0x00;
}

//
#define BUILD_CLANG_PARAM(LINE_TXT, COMPI_TOK, PARAM, CLAN_FILE) \
    int pind, lind; \
    LINE_TXT[0x00] = COMPI_TOK; LINE_TXT[0x01] = "-Wall"; \
    LINE_TXT[0x02] = "-fPIC"; LINE_TXT[0x03] = "-O2"; \
    LINE_TXT[0x04] = "-c"; LINE_TXT[0x05] = CLAN_FILE; \
    for(pind=0x06, lind=0x00; PARAM[lind]; lind++) { \
        if(!strncmp("-I", PARAM[lind], 0x02)) \
            LINE_TXT[pind++] = PARAM[lind]; \
    } \
    LINE_TXT[pind] = NULL;
//

int clan_compile(char *comp_param[], char *clan_file) {
    char *para_line[MAX_PARAM_CNT];
    //
    BUILD_CLANG_PARAM(para_line, GPP_COMPI_TOK, comp_param, clan_file)
    pid_t pid = vfork();
    if (0x00 > pid) {
        printf("fork g++ error!\n");
        return -1;
    } else if (!pid) {
        list_printf("g++ compiling\n", para_line);
        execvp(para_line[0x00], para_line);
        printf("g++ compile error.\n");
        exit(-1);
    }
    wait(NULL);
    //
    return 0x00;
}

//

int handle_execute(struct pcc_param *pcc_para) {
    clear_cache(PCC_CACHE_FILE);
    //
    int inde;
    for (inde = 0x00; pcc_para->cspa_file[inde]; inde++) {
        csp_compile(PCC_CACHE_FILE, pcc_para->cspa_file[inde]);
    }
    fill_middle(pcc_para->clan_file, PCC_CACHE_FILE);
    clear_cache(PCC_CACHE_FILE);
    //
    if (pcc_file_exis(pcc_para->clan_file)) return -1;
    for (inde = 0x00; pcc_para->clan_file[inde]; inde++) {
        clan_compile(pcc_para->comp_param, pcc_para->clan_file[inde]);
        fill_object(pcc_para->obj_file, pcc_para->clan_file[inde]);
    }
    //
    for (inde = 0x00; pcc_para->lib_file[inde]; inde++) {
        extract_lib(PCC_CACHE_FILE, pcc_para->lib_file[inde]);
    }
    fill_middle(pcc_para->obj_file, PCC_CACHE_FILE);
    clear_cache(PCC_CACHE_FILE);
    //
    if (pcc_file_exis(pcc_para->obj_file)) return -1;
    package_lib(pcc_para->comp_param, pcc_para->obj_file);
    //
    return 0;
}

void clear_midile(struct pcc_param *pcc_para) {
    printf("-------------- pcc --------------\n");
    printf("clear temp files.\n----- clear obj file.\n");
    clear_middle_file(pcc_para->obj_file);
    printf("----- clear clang file.\n");
    clear_middle_file(pcc_para->clan_file);
}

void dele_param_list(struct pcc_param *pcc_para) {
    int inde;
    for (inde = 0x00; pcc_para->cspa_file[inde]; inde++)
        free(pcc_para->cspa_file[inde]);
    for (inde = 0x00; pcc_para->clan_file[inde]; inde++)
        free(pcc_para->clan_file[inde]);
    for (inde = 0x00; pcc_para->obj_file[inde]; inde++)
        free(pcc_para->obj_file[inde]);
    for (inde = 0x00; pcc_para->lib_file[inde]; inde++)
        free(pcc_para->lib_file[inde]);
    for (inde = 0x00; pcc_para->comp_param[inde]; inde++)
        free(pcc_para->comp_param[inde]);
}

//

int main(int argc, char** argv) {
    if (0x02 > argc) {
        printf("no input file!\n");
        return EXIT_FAILURE;
    }
    struct pcc_param pcc_para;
    memset(&pcc_para, '\0', sizeof (struct pcc_param));
    printf("compilation starting ...\n");
    if (handle_param(&pcc_para, argc - 0x01, argv + 0x01))
        return (EXIT_FAILURE);
    //
    if (handle_execute(&pcc_para)) printf("compilation failed!\n");
    clear_midile(&pcc_para);
    dele_param_list(&pcc_para);
    printf("compilation terminated.\n");
    //
    return (EXIT_SUCCESS);
}

