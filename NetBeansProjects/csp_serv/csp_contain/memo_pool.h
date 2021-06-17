/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef MEMO_POOL_H
#define MEMO_POOL_H

#include "contain_macro.h"
#include "tiny_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

#define memo_pool_creat(tpool) tiny_create_pool((tpool)->pool_size)
#define close_memo_pool(tpool) tiny_destroy_pool(tpool)
    
#define CHECK_MEMO_POOL(avail, tpool, reset_rate) \
    if (avail) { \
        static int accap_loop; \
        if(!POW2N_MOD(++accap_loop, reset_rate)) tiny_reset_pool(tpool); \
    }

#ifdef __cplusplus
}
#endif

#endif /* MEMO_POOL_H */

