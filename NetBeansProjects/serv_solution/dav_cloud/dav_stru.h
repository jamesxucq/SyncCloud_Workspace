/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef DAV_STRU_H
#define DAV_STRU_H

#include "dav_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        char sin_addr[SERVNAME_LENGTH];
        int sin_port;
    } serv_addr;
    
    struct auth_config {
        serv_addr address;
        char database[DATABASE_LENGTH];
        char user_name[_USERNAME_LENGTH_];
        char password[PASSWORD_LENGTH];
    };

    //
    struct acce_value {
        unsigned int uid;
        uint32 pool_size;
        char access_key[KEY_LENGTH];
        serv_addr caddr;
        //
        unsigned int last_anchor;
        char location[MAX_PATH * 3];
        char req_valid[VALID_LENGTH];
        // append
        char _author_chks_[MD5_TEXT_LENGTH];
    };

    struct openod {
        unsigned int action_type; /* add mod del list recu */
        char file_name[MAX_PATH * 3];
        uint64 file_size;
        uint64 reserved;
    };
//
    struct auth_value {
        unsigned int uid;
        char password[PASSWORD_LENGTH];
        char _salt_[SALT_LENGTH];
    };

    //
    typedef struct {
        size_t      len;
        u_char     *data;
    } dav_str_t;
    
#define dav_string(str)     { sizeof(str) - 1, (u_char *) str }
    
    //
typedef struct {
    int max_modify;
    int modi_interv;
    int max_delete;
    int dele_interv;
} bakup_config;
    
    
#ifdef __cplusplus
}
#endif

#endif /* DAV_STRU_H */

