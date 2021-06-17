/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef ENCODING_H
#define ENCODING_H

#ifdef __cplusplus
extern "C" {
#endif

#define MD5_DIGEST_LEN 16
#define MD5_TEXT_LENGTH	33

//
char *trans_auth_chks(char *chks_str, unsigned int uid, char *access_key);
char *decode_base64_text(char *plain_txt, char *cipher_txt, char *salt_value);


#ifdef __cplusplus
}
#endif

#endif /* ENCODING_H */

