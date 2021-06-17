/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "dav_macro.h"
#include "parsing.h"
#include "auth_utili.h"
#include "encoding.h"
#include "auth_socks.h"
#include "anchor.h"
#include "capacity.h"
#include "logger.h"
#include "stri_utili.h"
#include "versi_utili.h"

#include "dav_cloud.h"
#include "dav_stru.h"

//
static uint64 _max_modi_siz_;
static uint64 _max_dele_siz_;

//
// __attribute__((constructor)) void init_enviro(void);
int init_enviro_ine (void) {
    bakup_config bakup_ctrl;
    debug_logger("inital enviro.\n");
    if (parse_config(&_anchor_files_, &_auth_config_, &bakup_ctrl)) {
        debug_logger("init parse config error!\n");
        return -1;
    }
    //
    _max_modi_siz_ = bakup_ctrl.max_modify << 20;
    _modi_interv_ = bakup_ctrl.modi_interv;
    debug_logger_dd("max_modify:%dMB modi_interv:%dsec\n", bakup_ctrl.max_modify, bakup_ctrl.modi_interv);
    _max_dele_siz_ = bakup_ctrl.max_delete << 20;
    _dele_interv_ = bakup_ctrl.dele_interv;
    debug_logger_dd("max_delete:%dMB dele_interv:%dsec\n", bakup_ctrl.max_delete, bakup_ctrl.dele_interv);
    debug_logger_ss("anch_idx:%s anch_acd:%s\n", _anchor_files_.anch_idx, _anchor_files_.anch_acd);
    debug_logger_ss("user_name:%s password:%s\n", _auth_config_.user_name, _auth_config_.password);
    debug_logger_s("database:%s\n", _auth_config_.database);
    debug_logger_sd("sin_addr:%s sin_port:%d\n", _auth_config_.address.sin_addr, _auth_config_.address.sin_port);
    //
    return 0x00;
}  

//
static char _acce_user_[_USERNAME_LENGTH_];
static struct acce_value _accval_;

//
#define SPLIT_USER(USER, USEPWD) \
    char usepwd[_USERNAME_LENGTH_]; \
    if(_USERNAME_LENGTH_ < USEPWD->len) { \
        debug_logger_s("user_pass:%s is too long\n", (char *)USEPWD->data); \
        return -1; \
    } \
    strcpy(usepwd, (char *)USEPWD->data); \
    char *delim = usepwd; \
    for (; '\0' != delim[0]; delim++) { \
        if (':' == delim[0]) { \
            delim[0] = '\0'; \
            break; \
        }; \
    } \
    USER = usepwd; \

//
// -1:error 0x00:ok 0x01:507 Insufficient Storage
int increm_locked_space(dav_str_t *user_pass) {
    char *user_name;
    SPLIT_USER(user_name, user_pass)
    //
debug_logger("increm locked space\n");
    if(strcmp(_acce_user_, user_name)) {
        if(query_access_bzl(&_accval_, user_name)) return -1;
        if(conne_cache_bzl(&_accval_.caddr)) return -1;
        strcpy(_acce_user_, user_name);
    }
    if(cache_locked_bzl(&_accval_)) return -1;
    if(check_capacity(_accval_.location, _accval_.pool_size)) {
        if(cache_unlock_bzl(_accval_.uid, _accval_.req_valid)) return -1;
        return 0x01;
    }
    //
    return 0x00;
}

// -1:error 0x00:ok
int locked_user_space(dav_str_t *user_pass) {
    char *user_name;
    SPLIT_USER(user_name, user_pass)
    //
debug_logger("locked user space\n");
    if(strcmp(_acce_user_, user_name)) {
        if(query_access_bzl(&_accval_, user_name)) return -1;
        if(conne_cache_bzl(&_accval_.caddr)) return -1;
        strcpy(_acce_user_, user_name);
    }
    if(cache_locked_bzl(&_accval_)) return -1;
    //
    return 0x00;
}

int unlock_user_space(dav_str_t *user_pass) {
    char *user_name;
    SPLIT_USER(user_name, user_pass)
    if(strcmp(_acce_user_, user_name)) return -1;
    //
debug_logger("unlock user space\n");
    if(!_accval_.last_anchor) {
        _accval_.last_anchor = load_last_anchor(_accval_.location);
        if(cache_anchor_bzl(_accval_.uid, _accval_.last_anchor, _accval_.req_valid)) 
            return -1;
    } else {
        if(cache_unlock_bzl(_accval_.uid, _accval_.req_valid)) 
            return -1;
    }
    return 0x00;
}

int update_anchor_openod(dav_str_t *user_pass, struct openod *opnod) {
    char *user_name;
    SPLIT_USER(user_name, user_pass)
    if(strcmp(_acce_user_, user_name)) return -1;
    //
debug_logger("update anchor openod\n");
    flush_anchor_bzl(_accval_.last_anchor, _accval_.location, opnod)
    reset_capacity(_accval_.location, opnod);
    if(cache_anchor_bzl(_accval_.uid, _accval_.last_anchor, _accval_.req_valid)) 
        return -1;
    //
    return 0x00;
}

int insert_openod(struct openod *opnod) {
debug_logger("insert openod\n");
    flush_action_bzl(_accval_.location, opnod);
    reset_capacity(_accval_.location, opnod);
    return 0x00;
}

int update_user_anchor(dav_str_t *user_pass) {
    char *user_name;
    SPLIT_USER(user_name, user_pass)
    if(strcmp(_acce_user_, user_name)) return -1;
    //
debug_logger("update user anchor\n");
    _accval_.last_anchor = addi_anchor_bzl(_accval_.location);
    if(cache_anchor_bzl(_accval_.uid, _accval_.last_anchor, _accval_.req_valid)) 
        return -1;
    //
    return 0x00;
}

int kalive_auth_cache() {
debug_logger("kalive auth cache\n");
    if(keep_alive_bzl(_accval_.uid, _accval_.req_valid)) return -1;
    return 0x00;;
}

// 0x00:ok -1:error
int cloud_delete(u_char *file_name, uint64 file_size) {
    char path_name[MAX_PATH * 3];
    //
debug_logger_s("cloud delete: %s\n", (char *)file_name);
    if (_max_dele_siz_ < file_size) {
        POOL_PATH(path_name, _accval_.location, file_name);
        unlink(path_name);
        debug_logger_s("unlink file:%s\n", (char *)file_name);
    } else {
        time_t act_time = time(NULL);
        int desta = delete_backup(_accval_.location, (char *)file_name, act_time);
        while (0x01 == desta) {
            desta = delete_backup(_accval_.location, (char *)file_name, act_time);
            debug_logger_d("desta:%d\n", desta);
            if (-1 == desta) {
                debug_logger_s("delete file error! %s\n", (char *)file_name);
                return -1;
            }
        }
    }
    //
    return 0x00;
}

//
static char _auth_name_[_USERNAME_LENGTH_];
static struct auth_value _autval_;
/**
* Get username and password, note that r->headers_in.user contains the
* string 'user:pass', so we need to copy the username
**/
#define SPLIT_USER_PASS(USER, PASS, USEPWD) \
    char usepwd[_USERNAME_LENGTH_]; \
    if(_USERNAME_LENGTH_ < USEPWD->len) { \
        debug_logger_s("user_pass:%s is too long\n", (char *)USEPWD->data); \
        return -1; \
    } \
    strcpy(usepwd, (char *)USEPWD->data); \
    char *delim = usepwd; \
    for (; '\0' != delim[0]; delim++) { \
        if (':' == delim[0]) { \
            delim[0] = '\0'; \
            break; \
        }; \
    } \
    USER = usepwd; PASS = delim + 0x01; \

int dav_authen(dav_str_t *user_pass) {
    char *user_name, *passwd;
    SPLIT_USER_PASS(user_name, passwd, user_pass)
debug_logger("dav authen\n");
debug_logger_ss("user_name:%s passwd:%s\n", user_name, passwd);
    if(strcmp(_auth_name_, user_name)) {
        if(query_authen_bzl(&_autval_, user_name)) return -1;
        strcpy(_auth_name_, user_name);
    }
// debug_logger_ss("_auth_name_:%s password:%s\n", user_name, _autval_.password);
    if(strcmp(passwd, _autval_.password)) {
        debug_logger_ss("not authenticated %s:%s\n", user_name, _autval_.password);
        return -1;
    }
//
    return 0x00;
}

