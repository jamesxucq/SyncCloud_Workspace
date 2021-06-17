/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef AUTH_UTILI_H
#define AUTH_UTILI_H

#include "dav_macro.h"
#include "dav_stru.h"
#include "parsing.h"

#ifdef __cplusplus
extern "C" {
#endif

//
extern struct auth_config _auth_config_;

//
int query_access_bzl(struct acce_value *accval, const char *user_name); 
int query_authen_bzl(struct auth_value *autval, const char *user_name); 

#ifdef __cplusplus
}
#endif

#endif /* AUTH_UTILI_H */

