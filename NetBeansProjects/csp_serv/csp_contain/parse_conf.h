/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef PARSE_CONF_H
#define PARSE_CONF_H

#include "contain_macro.h"
#include "virtu_path.h"
#include "access_log.h"
#include "logger.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
    } serv_addr;

    struct base_pool {
        int avail;
        serv_addr address;
        char database[DATABASE_LENGTH];
        char user_name[_USERNAME_LENGTH_];
        char password[PASSWORD_LENGTH];
        int recon_timeout;
    };

    struct memo_pool {
        int avail;
        int pool_size;
        int reset_rate;
    };

    typedef struct {
        char logfile[MAX_PATH * 3];
        log_lvl_t debug_level;
    } log_config;

    //

    struct cont_config {
        // memory pool
        struct memo_pool tpool;
        // mysql pool
        struct base_pool bpool;
        // log config
        log_config runtime_log;
        log_config access_log;
        // path map
        struct vpa_list *vlist;
    };

    //
    struct cont_config *load_config_bzl(const char *config_path);


#ifdef __cplusplus
}
#endif

#endif /* PARSE_CONF_H */

