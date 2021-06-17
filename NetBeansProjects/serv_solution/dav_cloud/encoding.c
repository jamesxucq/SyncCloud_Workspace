/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"
#include "dav_macro.h"
#include "base64.h"
#include "encoding.h"

//
#ifndef int32
#define int32 int
#endif

#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef uint64
#define uint64 unsigned long long
#endif

//
void get_md5sum_text(char *buf, int len, char *chks_str) {
    MD5_CTX md;

    MD5Init(&md);
    MD5Update(&md, (unsigned char *) buf, len);
    MD5Final(&md);

    uint32 inde;
    for (inde = 0; inde < MD5_DIGEST_LEN; inde++)
        sprintf(chks_str + inde * 2, "%02x", md.digest[inde]);
    chks_str[32] = '\0';
}


//
char *trans_auth_chks(char *chks_str, unsigned int uid, char *access_key) {
    char buffer[MID_TEXT_LEN];
    char chks[MD5_TEXT_LENGTH];
    //
    if (!access_key || !chks_str) return NULL;
    sprintf(buffer, "%u:%s", uid, access_key);
    get_md5sum_text(buffer, strlen(buffer), chks);
    //
    strcpy(chks_str, chks);
    return chks_str;
}

//
char *decode_base64_text(char *plain_txt, char *cipher_txt, char *salt_value) {
    char buffer[MID_TEXT_LEN];
    //
    if (!cipher_txt || !plain_txt || !salt_value || *cipher_txt == '\0') return NULL;
    buffer[0] = '\0';
    int desiz = base64_decode((unsigned char *) buffer, cipher_txt, MID_TEXT_LEN);
    int salt_length = strlen(salt_value);
    strncpy(plain_txt, buffer + salt_length, desiz - (salt_length << 1));
    plain_txt[desiz - (salt_length << 1)] = '\0';
    //
    return plain_txt;
}