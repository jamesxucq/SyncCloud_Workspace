
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "qmap.h"
//
unsigned int lh_strhash(void *src) {
    int i, l;
    unsigned long ret = 0;
    unsigned short *s;
    char *str = (char *) src;
    if (NULL == str) return (0);
    l = (strlen(str) + 1) / 2;
    s = (unsigned short *) str;

    for (i = 0; i < l; i++)
        ret ^= s[i] << (i & 0x0f);
    //
    return (ret);
}

int equal_str(void *k1, void *k2) {
    return (0 == strcmp((char *) k1, (char *) k2));
}

//

keymap *build_key_hmap(char *key_list[]) {
    keymap *kmap = khm_create(lh_strhash, equal_str, MATCH_BACKET_SIZE);
    if(!kmap) {
        _LOG_FATAL("build_path_hmap khm_create error.");
        return NULL;
    }
    //
    char **key_word = key_list;
    for (; *key_word; key_word++) {
// printf("kmap:%08X key_word:|%s|\n", kmap, *key_word);
        khm_insert(*key_word, (void *)-1, kmap);
    }
    //
    return kmap;
}

void destory_key_hmap(keymap *kmap) {
    khm_dele(kmap);
}

//
