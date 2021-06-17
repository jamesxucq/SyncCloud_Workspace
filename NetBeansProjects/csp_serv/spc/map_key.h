/* 
 * File:   map_key.h
 * Author: Administrator
 */

#ifndef MAP_KEY_H
#define	MAP_KEY_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct khm_nod {
    void *key;
    void *data;
    struct khm_nod *next;
};

struct _keymap_ {
    unsigned int (*gethash)(void *);
    int (*compare)(void *, void *);
    unsigned int hashsize;
    unsigned int count;
    struct khm_nod **hmaplist;
};

typedef struct _keymap_ keymap;

keymap *khm_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size);
void khm_dele(keymap *kmap);
void khm_insert(void *key, void *data, keymap *kmap);
void khm_remove(void *key, keymap *kmap);
void *khm_value(void *key, keymap *kmap);
void khm_for_each_do(keymap *kmap, int (call_back) (void *, void *));
int khm_count(keymap *kmap);

#ifdef	__cplusplus
}
#endif

#endif	/* MAP_KEY_H */

