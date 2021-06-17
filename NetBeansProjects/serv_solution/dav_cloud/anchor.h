/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ANCHOR_H
#define ANCHOR_H

#include "dav_macro.h"
#include "dav_stru.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char anch_idx[MAX_PATH * 3];
    char anch_acd[MAX_PATH * 3];
} anchor_files;

//
extern anchor_files _anchor_files_;

//
uint32 load_last_anchor(char *location);
uint32 addi_anchor(char *location);
int flush_action(char *location, struct openod *opesdo);

#define flush_anchor_bzl(ins_anchor, location, opesdo) \
    flush_action(location, opesdo); \
    addi_anchor(location); \
    debug_logger("flush_anchor_bzl\n");

#define flush_action_bzl(location, opesdo) flush_action(location, opesdo)
#define addi_anchor_bzl(location) addi_anchor(location)

#ifdef __cplusplus
}
#endif

#endif /* ANCHOR_H */

