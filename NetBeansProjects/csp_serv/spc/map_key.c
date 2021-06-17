#include <stdlib.h>
#include <string.h>

#include "map_key.h"

#define hashinde(key, kmap) ((kmap->gethash)(key) % (kmap->hashsize))

inline struct khm_nod *pnod_new(void *key, void *data) {
    struct khm_nod *pnod = malloc(sizeof (struct khm_nod));
    if(!pnod) return NULL;
    pnod->key = key;
    pnod->data = data;
    pnod->next = NULL;
    return pnod;
}

void klist_append(struct khm_nod **root, void *key, void *data) {
    struct khm_nod *new, *posi;
    //
    new = pnod_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int klist_update(struct khm_nod *root, void *key, void *data, int (*compare)(void *, void *)) {
    struct khm_nod *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (compare(key, posi->key)) {
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct khm_nod *pnod_del(struct khm_nod *nod) {
    struct khm_nod *next = nod->next;
    free(nod);
    return (next);
}

int klist_remove(struct khm_nod **root, void *key, int (*compare)(void *, void *)) {
    struct khm_nod *posi, *prev;
    if (NULL == *root) return -1;
    if (compare((*root)->key, key)) {
        *root = pnod_del(*root);
        return 0;
    }
    //
    prev = *root;
    for (posi = prev->next; NULL != posi; posi = posi->next) {
        if (compare(posi->key, key)) {
            prev->next = pnod_del(posi);
            return 0;
        }
        prev = posi;
    }
    return -1;
}

keymap *khm_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size) {
    int length = sizeof (struct khm_nod *) * size;
    //
    keymap *kmap = (keymap *) malloc(sizeof (keymap));
    if(!kmap) return NULL;
    memset(kmap, '\0', sizeof (keymap));
    kmap->gethash = keyfunc;
    kmap->compare = compare;
    kmap->hashsize = size;
    kmap->count = 0;
    //
    kmap->hmaplist = malloc(length);
    if (NULL == kmap->hmaplist) {
        free(kmap);
        return NULL;
    }
    memset(kmap->hmaplist, '\0', length);
    //
    return kmap;
}

void khm_dele(keymap *kmap) {
    unsigned int inde;
    struct khm_nod *posi;
    //
    if (NULL == kmap) return;
    for (inde = 0; inde < kmap->hashsize; inde++)
        for (posi = kmap->hmaplist[inde]; NULL != posi; posi = pnod_del(posi));
    //
    free(kmap->hmaplist);
    free(kmap);
}

void khm_insert(void *key, void *data, keymap *kmap) {
    unsigned int inde = hashinde(key, kmap);
    struct khm_nod *root = kmap->hmaplist[inde];
    //
    if (0 != klist_update(root, key, data, kmap->compare)) { //(1)
        klist_append(&(kmap->hmaplist[inde]), key, data);
        kmap->count++;
    }
}

void khm_remove(void *key, keymap *kmap) {
    unsigned int inde = hashinde(key, kmap);
    if (0 == klist_remove(&(kmap->hmaplist[inde]), key, kmap->compare)) {
        kmap->count--;
    }
}

void *khm_value(void *key, keymap *kmap) {
    struct khm_nod *posi;
    unsigned int inde = hashinde(key, kmap);
    for (posi = kmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
        if (kmap->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

void khm_for_each_do(keymap *kmap, int (call_back) (void *, void *)) {
    unsigned int inde = 0;
    struct khm_nod *posi;
    for (inde = 0; inde < kmap->hashsize; inde++) {
        for (posi = kmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int khm_count(keymap *kmap) {
    return kmap->count;
}

