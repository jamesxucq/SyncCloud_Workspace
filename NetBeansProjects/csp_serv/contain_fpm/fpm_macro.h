/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fpm_macro.h
 * Author: Administrator
 *
 * Created on 2016.12.1, AM 9:18
 */

#ifndef CFPM_MACRO_H
#define CFPM_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif
    
//
#define MKZERO(TEXT) TEXT[0] = '\0'
    
//
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
    
//
#define SERVNAME_LENGTH			256
#define _USERNAME_LENGTH_               128
#define GROUPNAME_LENGTH                128

//
#define CSPFPM_DEFAULT "csp_contain.xml"
#define CSPFPM_PID_DEFAULT "csp_contain.pid"
#define CSPFPM_LOG_DEFAULT "csp_contain.log"
#define CFPM_USER_DEFAULT "nobody"
#define CFPM_GROUP_DEFAULT "nobody"
#define CFPM_CONTAIN_DEFAULT "csp_contain"


#ifdef __cplusplus
}
#endif

#endif /* CFPM_MACRO_H */

