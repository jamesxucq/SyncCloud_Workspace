/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef PARSING_H
#define PARSING_H

#include "dav_stru.h"
#include "anchor.h"
#include "auth_utili.h"

#ifdef __cplusplus
extern "C" {
#endif

//
#define DAV_CLOUD_DEFAULT   "dav_cloud.conf"
    
//
int parse_config(anchor_files *afiles, struct auth_config *config, bakup_config *bakup_ctrl);


#ifdef __cplusplus
}
#endif

#endif /* PARSING_H */

