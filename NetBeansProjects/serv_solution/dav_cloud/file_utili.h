/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <errno.h>
#include <utime.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#ifndef FILE_UTILI_H
#define FILE_UTILI_H

#ifdef __cplusplus
extern "C" {
#endif

#define FILE_SIZE(SIZE, PATH) { \
    struct stat64 statb; \
    if (stat64(PATH, &statb) < 0) \
        debug_logger_d("stat errno:%d\n", errno); \
    SIZE = statb.st_size; \
}
    //off_t file_size_stre(FILE *stre);
#define FILE_SIZE_STREAM(SIZE, STREAM) { \
    if (fseeko64(STREAM, 0, SEEK_END)) \
        debug_logger_d("fseeko64 errno:%d\n", errno); \
    SIZE = ftello64(STREAM); \
}

#define FILE_GET_DIRE(FILE_DIRE, FILE_PATH) { \
    char path[PATH_MAX]; \
    strcpy(path, FILE_PATH); \
    strcpy(FILE_DIRE, dirname(path)); \
}
    
//
#define CREAT_BAKUP_DIRPATH(BAKUP_PATH, MODE) { \
    char filedir[PATH_MAX]; \
    FILE_GET_DIRE(filedir, BAKUP_PATH) \
    if (mkdir(filedir, MODE)) \
        debug_logger_d("mkdir errno:%d\n", errno); \
    debug_logger_s("mkdir bakup path:%s\n", filedir); \
}
    
//
int file_copy_ext(char *from, char *to);

char *get_appli_path();
#define CREAT_CONF_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/conf/%s", get_appli_path(), FILE_NAME);

#ifdef __cplusplus
}
#endif

#endif /* FILE_UTILI_H */

