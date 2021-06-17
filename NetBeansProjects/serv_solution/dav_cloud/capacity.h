/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CAPACITY_H
#define CAPACITY_H

#include "dav_macro.h"
#include "dav_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CAPACITY_DEFAULT   "~/capacity.acd"

    //
    struct capacity {
        uint64 used_size;
        uint64 bakup_size;
    };

    //
    
    int reset_capacity(char *location, struct openod *opesdo);
    // 0:ok -1:error 0x01:exception
    int check_capacity(char *location, uint32 pool_size);

#ifdef __cplusplus
}
#endif

#endif /* CAPACITY_H */

