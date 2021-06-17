/* 
 * File:   file_utili.h
 * Author: Administrator
 *
 * Created on 2016.12.1, AM 8:47
 */

#ifndef FILE_UTILI_H
#define FILE_UTILI_H

#ifdef __cplusplus
extern "C" {
#endif

    //
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

    //
#define FILE_EXIST(PATH_NAME) access(PATH_NAME, F_OK)

    //
    size_t write_all(int fildes, const void *buf, size_t nbyte);
    int open_pid_file(char *pid_file);
    //
    char *get_appli_path();
#define CREAT_CONF_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/conf/%s", get_appli_path(), FILE_NAME);
#define CREAT_LOGS_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/logs/%s", get_appli_path(), FILE_NAME);
#define CREAT_SBIN_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/sbin/%s", get_appli_path(), FILE_NAME);

#ifdef __cplusplus
}
#endif

#endif /* FILE_UTILI_H */

