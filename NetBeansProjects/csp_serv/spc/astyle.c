#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "astyle.h"

//
#define CLANG_ASTYLE     "astyle"

//
static int _format_;

//

void set_astyle_enable(int enable) {
    _format_ = enable;
}

//

int format_clang_style(struct built_list *midd_list) {
    if(!_format_) return -1;
    printf("format built file!\n");
    //
    char *para_line[0x05] = {CLANG_ASTYLE, "--style=kr", "-n", NULL, NULL};
    int inde;
    pid_t pid;
    char **clist = midd_list->clan_file;
    for (inde = 0x00; clist[inde]; inde++) {
        para_line[0x03] = clist[inde];
        pid = vfork();
        if (0x00 > pid) {
            printf("fork astyle error!\n");
            return -1;
        } else if (!pid) {
            printf("format %s.\n", clist[inde]);
            execvp(CLANG_ASTYLE, para_line);
            printf("format error.\n");
            exit(-1);
        }
        wait(NULL);
    }
    //
    return 0x00;
}