/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef QMAP_H
#define QMAP_H

#include "map_key.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATCH_BACKET_SIZE     0x80000  // 512K
    
//

unsigned int lh_strhash(void *src);
int equal_str(void *k1, void *k2);

//
keymap *build_key_hmap(char *key_list[]);
void destory_key_hmap(keymap *kmap);

//

#ifdef __cplusplus
}
#endif

#endif /* QMAP_H */

