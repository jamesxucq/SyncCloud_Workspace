/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef CSP_MACRO_H
#define CSP_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif

//
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
    
//
#define POW2N_MOD(nua, nub)  (nua & (nub -1)) // x=2^n a=b%x-->a=b&(x-1)    

//
#define SERVNAME_LENGTH			256
#define DATABASE_LENGTH			128
#define _USERNAME_LENGTH_               128
#define PASSWORD_LENGTH			128

//
#define CSP_CONTAIN_DEFAULT "csp_contain.xml"
#define CONTAIN_LOG_DEFAULT "csp_contain.log"
#define CONTAIN_ACCESS_DEFAULT "csp_access.log"


#ifdef __cplusplus
}
#endif

#endif /* CSP_MACRO_H */

