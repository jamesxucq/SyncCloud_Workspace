/* 
 * File:   parse_conf.h
 * Author: Administrator
 *
 * Created on 2016.12.1, AM 9:00
 */

#ifndef PARSE_CONF_H
#define PARSE_CONF_H

#include "logger.h"
#include "fpm_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
    } serv_addr;

    typedef struct {
        char logfile[MAX_PATH * 3];
        log_lvl_t debug_level;
    } log_config;

    //

    struct fpm_config {
        char fcgi_app[MAX_PATH * 3];
        char user_name[_USERNAME_LENGTH_];
        char group_name[GROUPNAME_LENGTH];
        serv_addr address;
        char unix_domain[MAX_PATH * 3];
        int fork_count;
        char pid_file[MAX_PATH * 3];
        char work_directory[MAX_PATH * 3];
        // log config
        log_config runtime_log;
    };

    //
    struct fpm_config *load_config_bzl(const char *config_path);
    int check_config(struct fpm_config *config);

#ifdef __cplusplus
}
#endif

#endif /* PARSE_CONF_H */

