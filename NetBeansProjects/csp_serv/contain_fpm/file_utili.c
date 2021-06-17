#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "logger.h"
#include "file_utili.h"
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

//
size_t write_all(int fildes, const void *buf, size_t nbyte) {
    size_t rem;
    for (rem = nbyte; rem > 0;) {
        ssize_t res = write(fildes, buf, rem);
        if (-1 == res) {
            if (EINTR != errno) return res;
        } else {
            buf = res + (char const*) buf;
            rem -= res;
        }
    }
    return nbyte;
}

//
int open_pid_file(char *pid_file) {
    int pid_fd = -1;
    //
    if (pid_file[0] && (-1 == (pid_fd = open(pid_file, O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)))) {
        struct stat st;
        if (errno != EEXIST) {
            _LOG_WARN("csp-fpm: opening PID-file '%s' failed: %s\n", pid_file, strerror(errno));
            return -1;
        }
        /* ok, file exists */
        if (0 != stat(pid_file, &st)) {
            _LOG_WARN("csp-fpm: stating PID-file '%s' failed: %s\n", pid_file, strerror(errno));
            return -1;
        }
        /* is it a regular file ? */
        if (!S_ISREG(st.st_mode)) {
            _LOG_WARN("csp-fpm: PID-file exists and isn't regular file: '%s'\n", pid_file);
            return -1;
        }
        if (-1 == (pid_fd = open(pid_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))) {
            _LOG_WARN("csp-fpm: opening PID-file '%s' failed: %s\n", pid_file, strerror(errno));
            return -1;
        }
    }
    //
    return pid_fd;
}