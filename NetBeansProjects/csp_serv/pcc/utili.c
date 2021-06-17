#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utili.h"

//

int cspla_to_clang(char *lang) {
    char *suffix = strrchr(lang, '.');
    if (!suffix) return -1;
    if (!strcasecmp(suffix, ".csp")) {
        *(++suffix) = 'c';
        *(++suffix) = 'p';
        *(++suffix) = 'p';
        *(++suffix) = '\0';
    }
    return 0;
}

int fill_middle(char *midd_file[], char *file_name) {
    FILE *midd_fp = fopen64(file_name, "rb");
    if (!midd_fp) return -1;
    fseeko64(midd_fp, 0x00, SEEK_SET);
    //
    int inde;
    for (inde = 0x00; midd_file[inde]; inde++);
    char name_value[MAX_PATH];
    for (; fgets(name_value, MAX_PATH, midd_fp); inde++) {
        char *toke = strchr(name_value, '\n');
        if(toke) *toke = '\0';
        midd_file[inde] = strdup(name_value);
    }
    midd_file[inde] = NULL;
    //
    fclose(midd_fp);
    return 0x00;
}

char *clan_to_obj(char *clan) {
    char *obj_file = strdup(clan);
    char *suffix = strrchr(obj_file, '.');
    if (!suffix) {
        free(obj_file);
        return NULL;
    }
    if (!strcasecmp(suffix, ".cpp")) {
        *(++suffix) = 'o';
        *(++suffix) = '\0';
    }
    return obj_file;
}

int fill_object(char *obj_file[], char *clan_file) {
    int inde;
    for (inde = 0x00; obj_file[inde]; inde++);
    obj_file[inde] = clan_to_obj(clan_file);
    obj_file[++inde] = NULL;
    //
    return 0x00;
}

//

void trans_page_suffix(char *cspa_file[]) {
    int inde;
    for (inde = 0x00; cspa_file[inde]; inde++)
        cspla_to_clang(cspa_file[inde]);
}

int pcc_file_exis(char *pcc_file[]) {
    int chk_val = 0x00;
    //
    int inde;
    for (inde = 0x00; pcc_file[inde]; inde++) {
        if (access(pcc_file[inde], F_OK)) {
            if (!chk_val) {
                printf("build clang failed!\n");
                chk_val++;
            }
            printf("%s ", pcc_file[inde]);
        }
    }
    if (chk_val) printf("not exists.\n");
    //
    return chk_val;
}

//
void clear_middle_file(char *middle_file[]) {
    int inde;
    for (inde = 0x00; middle_file[inde]; inde++) {
        unlink(middle_file[inde]);
printf("unlink:%s\n", middle_file[inde]);
    }
}

void clear_cache(char *cache_file) {
    unlink(cache_file);
    // printf("unlink:%s\n", cspa_file[inde]);
}
