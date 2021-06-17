/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include "logger.h"

#define MAX_COPY_SIZ  0x20000 // 1GB
// #define MAX_COPY_SIZ  1024 // 8MB

int file_copy_ext(char *from, char *to) { // 0:ok 1:processing -1:error
    struct stat64 statbuf1, statbuf2;
    if (stat64(from, &statbuf1) == -1) {
        debug_logger_s("can not get info of %s\n", from);
        return -1;
    }
    if (stat64(to, &statbuf2) == -1)
        statbuf2.st_size = 0;
    //
    int fd1, fd2;
    if ((fd1 = open64(from, O_RDONLY)) == -1) {
        debug_logger_s("can not open file %s\n", from);
        return -1;
    }
    if ((fd2 = open64(to, O_WRONLY | O_CREAT | O_APPEND, statbuf1.st_mode)) == -1) {
        close(fd1);
        debug_logger_s("can not open file %s\n", to);
        return -1;
    }
    //
    int size, inde;
    unsigned char buffer[0x2000] = ""; //8K
    int cpval = 0x01;
    lseek64(fd1, statbuf2.st_size, SEEK_SET);
    for (inde = 0; MAX_COPY_SIZ > inde; inde++) {
        if ((size = read(fd1, buffer, 0x2000))) {
            if (write(fd2, buffer, size) != size) {
                cpval = -1;
                debug_logger("write error!\n");
                break;
            }
        } else {
            cpval = 0x00;
            break;
        }
    }
    //
    close(fd1);
    close(fd2);
    return cpval;
}

//
char *get_executable_path(char* epath) {
    char exec_path[PATH_MAX];
    int len = readlink("/proc/self/exe", exec_path, PATH_MAX);
    if (0 > len) return NULL;
    exec_path[len] = '\0';
    //
    strcpy(epath, exec_path);
    return epath;
}

char *get_appli_path() {
    static char appli_path[1024];
    if ('\0' == appli_path[0]) {
        if (!get_executable_path(appli_path)) return NULL;
        char *tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        tokep[0] = '\0';
        tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        (tokep)[0] = '\0';
    }
    return appli_path;
}