/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef AUTH_SOCKS_H
#define AUTH_SOCKS_H

//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dav_macro.h"
#include "dav_stru.h"
#include "auth_reply.h"

#ifdef __cplusplus
extern "C" {
#endif
//
extern serv_addr _cache_saddr_;
extern int _cache_sockfd_;

//    
int set_cache_anchor(unsigned int uid, unsigned int last_anchor, char *req_valid, int cache_sockfd);
#define cache_anchor_bzl(uid, last_anchor, req_valid) \
    set_cache_anchor(uid, last_anchor, req_valid, _cache_sockfd_)

int set_cache_locked(struct acce_value *accval, int cache_sockfd);
#define cache_locked_bzl(accval) \
    set_cache_locked(accval, _cache_sockfd_)

int set_cache_unlock(unsigned int uid, char *req_valid, int cache_sockfd);
#define cache_unlock_bzl(uid, req_valid) \
    set_cache_unlock(uid, req_valid, _cache_sockfd_)

int keep_alive_cache(unsigned int uid, char *req_valid, int cache_sockfd);
#define keep_alive_bzl(uid, req_valid) \
    keep_alive_cache(uid, req_valid, _cache_sockfd_)
    
////////////////////////////////////////////////////////////////////////////////
int conne_cache_bzl(serv_addr *caddr);


#ifdef __cplusplus
}
#endif

#endif /* AUTH_SOCKS_H */

