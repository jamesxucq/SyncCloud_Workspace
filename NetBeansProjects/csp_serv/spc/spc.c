
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "common.h"
#include "logger.h"
#include "compile.h"
#include "middle.h"
#include "utility.h"
#include "parser.h"
#include "conv.h"
#include "builder.h"
#include "inte.h"
#include "astyle.h"


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
#define INVA_FILE_TYPE      -1
#define CSPA_FILE_TYPE      0x0001
#define COMPI_PARAM_TYPE    0x0003

int param_type(char *str) {
    if ('-' == str[0x00]) return COMPI_PARAM_TYPE;
    char *suffix = strrchr(str, '.');
    if (!suffix) return COMPI_PARAM_TYPE;
    else if (!strcasecmp(suffix, ".csp")) return CSPA_FILE_TYPE;
    //
    return INVA_FILE_TYPE;
}

//

int handle_param(struct spc_param *spc_para, int numbe, char *param[]) {
    int csp_inde = 0x00;
    int inde;
    for (inde = 0x00; numbe > inde; inde++) {
        switch (param_type(param[inde])) {
            case CSPA_FILE_TYPE:
                spc_para->cspa_file[csp_inde++] = strdup(param[inde]);
                break;
            case COMPI_PARAM_TYPE:
                if (!strcmp("-p", param[inde])) spc_para->function = 0x01;
                else if (!strcmp("-a", param[inde])) set_astyle_enable(0x01);
                else if (!strcmp("-s", param[inde])) set_middle_file(param[++inde]);
                break;
        }
    }
    spc_para->cspa_file[csp_inde] = NULL;
    //
    if (spc_file_exis(spc_para->cspa_file)) return -1;
    list_printf("cspa_file", spc_para->cspa_file);
    //
    if (!csp_inde) {
        printf("no input file!\n");
        return -1;
    }
    //
    return 0x00;
}

//

void dele_param_list(struct spc_param *spc_para) {
    int inde;
    for (inde = 0x00; spc_para->cspa_file[inde]; inde++)
        free(spc_para->cspa_file[inde]);
}

//

int main(int argc, char** argv) {
    if (0x02 > argc) {
        printf("no input file!\n");
        return EXIT_FAILURE;
    }
    struct spc_param spc_para;
    memset(&spc_para, '\0', sizeof (struct spc_param));
    if (handle_param(&spc_para, argc - 0x01, argv + 0x01))
        return (EXIT_FAILURE);
    //
    struct built_list midd_list;
    open_middle_file(&midd_list);
    if (handle_compile(&midd_list, &spc_para)) printf("compilation failed!\n");
    dele_param_list(&spc_para);
    //
    flush_midd_file(&midd_list);
    format_clang_style(&midd_list);
    dele_midd_list(&midd_list);
    printf("compilation terminated.\n");
    ///////////////////////    
    return (EXIT_SUCCESS);
}
