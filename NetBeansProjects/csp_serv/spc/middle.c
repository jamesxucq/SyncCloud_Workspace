#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "middle.h"

//    
static char _midd_name_[MAX_PATH];
static int _middle_;

//

void set_middle_file(char *midd_name) {
    _middle_ = 0x01;
    strcpy(_midd_name_, midd_name);
    printf("set middle file:%s!\n", _midd_name_);
}

//
#define INVA_FILE_TYPE      -1
#define CLAN_FILE_TYPE      0x0001

static int clang_type(char *str) {
    char *suffix = strrchr(str, '.');
    if (!suffix) return INVA_FILE_TYPE;
    else if (!strcasecmp(suffix, ".cpp")) return CLAN_FILE_TYPE;
    return INVA_FILE_TYPE;
}

static char **fill_middle(char *midd_file[], char *file_name, int clan_type) {
    FILE *midd_fp = fopen64(file_name, "rb");
    if (!midd_fp) return NULL;
    fseeko64(midd_fp, 0x00, SEEK_SET);
    //
    int inde;
    char name_value[MAX_PATH];
    for (inde = 0x00; fgets(name_value, MAX_PATH, midd_fp);) {
        // printf("name_value:%s clang_type:%d\n", name_value, clang_type(name_value));
        char *toke = strchr(name_value, '\n');
        if (toke) *toke = '\0';
        if (clan_type == clang_type(name_value)) midd_file[inde++] = strdup(name_value);
    }
    midd_file[inde] = NULL;
    //
    fclose(midd_fp);
//
    return (midd_file + inde);
}

void open_middle_file(struct built_list *midd_list) {
    memset(midd_list, '\0', sizeof (struct built_list));
    if (_middle_) {
        midd_list->last_clan = fill_middle(midd_list->clan_file, _midd_name_, CLAN_FILE_TYPE);
        if (!midd_list->last_clan) midd_list->last_clan = midd_list->clan_file;
        printf("last_clan:%08x clan_file:%08x\n", midd_list->last_clan, midd_list->clan_file);
    }
}

//

int flush_midd_file(struct built_list *midd_list) {
    if (!_middle_) return -1;
    printf("build middle file!\n");
    //
    FILE *midd_fp = fopen64(_midd_name_, "ab");
    if (!midd_fp) return -1;
    int inde;
    char **clist = midd_list->clan_file;
    for (inde = 0x00; midd_list->last_clan != (clist + inde); inde++);
    for (; clist[inde]; inde++) {
        fputs(clist[inde], midd_fp);
        fputc('\n', midd_fp);
        printf("middle file, %s\n", clist[inde]);
    }
    if (midd_fp) fclose(midd_fp);
    //
    return 0x00;
}

//

void dele_midd_list(struct built_list *midd_list) {
    int inde;
    char **clan_name = midd_list->clan_file;
    for (inde = 0x00; clan_name[inde]; inde++)
        free(clan_name[inde]);
}

//

int put_midd_file(struct built_list *midd_list, char *file_name) {
    int clan_type = clang_type(file_name);
    char **clan_name;
// printf("put middle file, %s\n", file_name);
    int inde;
    if (CLAN_FILE_TYPE == clan_type) {
        clan_name = midd_list->clan_file;
        for (inde = 0x00; clan_name[inde]; inde++) {
            if (!strcmp(file_name, clan_name[inde])) {
                return -1;
            }
        }
        clan_name[inde++] = strdup(file_name);
        clan_name[inde] = NULL;
    }
    //
    return 0x00;
}