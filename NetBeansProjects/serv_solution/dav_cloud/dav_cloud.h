/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef DAV_CLOUD_H
#define DAV_CLOUD_H

#include "dav_macro.h"
#include "dav_stru.h"

#ifdef __cplusplus
extern "C" {
#endif
    
//
#define GetTickCount time(NULL);
    
//
int increm_locked_space(dav_str_t *user_pass);
int locked_user_space(dav_str_t *user_pass);
int unlock_user_space(dav_str_t *user_pass);
int update_anchor_openod(dav_str_t *user_pass, struct openod *opnod);
int insert_openod(struct openod *opnod);
int update_user_anchor(dav_str_t *user_pass);
int kalive_auth_cache();
int cloud_delete(u_char *file_name, uint64 file_size);

//
#define increm_locked_ine(usep) increm_locked_space((dav_str_t *)usep)
#define locked_space_ine(usep) locked_user_space((dav_str_t *)usep)
#define unlock_space_ine(usep) unlock_user_space((dav_str_t *)usep)
#define update_anchor_ine(usep) update_user_anchor((dav_str_t *)usep)
#define cache_kalive_ine kalive_auth_cache

//
#define addi_update_ine(usep, name, size) { \
    struct openod opnod; \
    opnod.action_type = ADDI; \
    strcpy(opnod.file_name, (char *)name); \
    opnod.file_size = size; \
    update_anchor_openod((dav_str_t *)usep, &opnod); \
}

#define dele_insert_ine(name, size) { \
    struct openod opnod; \
    opnod.action_type = DELE; \
    strcpy(opnod.file_name, (char *)name); \
    opnod.file_size = size; \
    insert_openod(&opnod); \
}

#define move_insert_ine(exnam, mvnam) { \
    struct openod opnod; \
    opnod.file_size = 0x00; \
    opnod.reserved = GetTickCount; \
    opnod.action_type = EXIST; \
    strcpy(opnod.file_name, (char *)exnam); \
    insert_openod(&opnod); \
    opnod.action_type = MOVE; \
    strcpy(opnod.file_name, (char *)mvnam); \
    insert_openod(&opnod); \
}

#define move_update_ine(usep, exnam, mvnam) { \
    struct openod opnod; \
    opnod.file_size = 0x00; \
    opnod.reserved = GetTickCount; \
    opnod.action_type = EXIST; \
    strcpy(opnod.file_name, (char *)exnam); \
    insert_openod(&opnod); \
    opnod.action_type = MOVE; \
    strcpy(opnod.file_name, (char *)mvnam); \
    update_anchor_openod((dav_str_t *)usep, &opnod); \
}

#define copy_insert_ine(exnam, mvnam, size) { \
    struct openod opnod; \
    opnod.file_size = size; \
    opnod.reserved = GetTickCount; \
    opnod.action_type = EXIST; \
    strcpy(opnod.file_name, (char *)exnam); \
    insert_openod(&opnod); \
    opnod.action_type = COPY; \
    strcpy(opnod.file_name, (char *)mvnam); \
    insert_openod(&opnod); \
}

#define copy_update_ine(usep, exnam, mvnam, size) { \
    struct openod opnod; \
    opnod.file_size = size; \
    opnod.reserved = GetTickCount; \
    opnod.action_type = EXIST; \
    strcpy(opnod.file_name, (char *)exnam); \
    insert_openod(&opnod); \
    opnod.action_type = COPY; \
    strcpy(opnod.file_name, (char *)mvnam); \
    update_anchor_openod((dav_str_t *)usep, &opnod); \
}

//
int init_enviro_ine (void);
int dav_authen(dav_str_t *user_pass);
#define dav_authen_ine(usep) dav_authen((dav_str_t *)usep)

#ifdef __cplusplus
}
#endif

#endif /* DAV_CLOUD_H */

