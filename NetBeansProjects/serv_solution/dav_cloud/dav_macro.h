/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef DAV_MACRO_H
#define DAV_MACRO_H

#ifdef __cplusplus
extern "C" {
#endif
    
////////////////////////////////////////////////////////////////////////////////
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
    
////////////////////////////////////////////////////////////////////////////////
#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uint64
#define uint64 unsigned long
#endif
    
#ifndef MAX_PATH
#define MAX_PATH 260
#endif

//
#define TIME_LENGTH             32
    
//
#define POW2N_MOD(nua, nub)  (nua & (nub -1)) // x=2^n a=b%x-->a=b&(x-1)
#define s_strlen(str) (sizeof(str) - 1)
#define MKZERO(TEXT) TEXT[0] = '\0'
    
////////////////////////////////////////////////////////////////////////////////
#define INVA_ATTR		0x00000000
// file real status
#define ADDI                    0x00000001 // addition
#define DELE                    0x00000002 // delete
#define MODIFY                  0x00000004 // modify
#define EXIST                   0x00000008 // exist
#define COPY                    0x00000010 // copy
#define MOVE                    0x00000020 // move

//
#define CONTENT_TYPE_INVA               0x0000
#define CONTENT_TYPE_XML                0x0001
#define CONTENT_TYPE_OCTET              0x0002
#define CONTENT_TYPE_PLAIN              0x0003
    
////////////////////////////////////////////////////////////////////////////////
#define AUTH_TEXT_LEN                   0x2000  // 8K
#define AUTH_LINE_LEN                   512
#define AUTH_SOCKS_DATSIZ               1024
    
#define MAX_TEXT_LEN		512
#define MID_TEXT_LEN		128
#define MIN_TEXT_LEN		64
    
#define SERVNAME_LENGTH			256
#define DATABASE_LENGTH			128
#define _USERNAME_LENGTH_               128
#define PASSWORD_LENGTH			128
#define SALT_LENGTH			32

#define CONTENT_TYPE_LENGTH		32
#define KEY_LENGTH			32    
#define VALID_LENGTH                    32
#define DIGIT_LENGTH                    32
#define EXECUTE_LENGTH                  32

#ifndef MD5_TEXT_LENGTH
#define MD5_TEXT_LENGTH                 33
#endif

//

#ifdef __cplusplus
}
#endif

#endif /* DAV_MACRO_H */

