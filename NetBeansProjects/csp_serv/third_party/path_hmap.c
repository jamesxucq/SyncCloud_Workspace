#include <stdlib.h>
#include <string.h>

#include "path_hmap.h"

#define hashinde(key, pmap) ((pmap->gethash)(key) % (pmap->hashsize))

inline struct phm_nod *pnod_new(void *key, void *data) {
    struct phm_nod *pnod = malloc(sizeof (struct phm_nod));
    if(!pnod) return NULL;
    pnod->key = key;
    pnod->data = data;
    pnod->next = NULL;
    return pnod;
}

void vlist_append(struct phm_nod **root, void *key, void *data) {
    struct phm_nod *new, *posi;
    //
    new = pnod_new(key, data);
    if (NULL == *root) {
        *root = new;
    } else {
        for (posi = *root; NULL != posi->next; posi = posi->next);
        posi->next = new;
    }
}

int vlist_update(struct phm_nod *root, void *key, void *data, int (*compare)(void *, void *)) {
    struct phm_nod *posi;
    for (posi = root; NULL != posi; posi = posi->next) {
        if (compare(key, posi->key)) {
            posi->data = data;
            return 0;
        }
    }
    return -1;
}

inline struct phm_nod *pnod_del(struct phm_nod *nod) {
    struct phm_nod *next = nod->next;
    free(nod);
    return (next);
}

int vlist_remove(struct phm_nod **root, void *key, int (*compare)(void *, void *)) {
    struct phm_nod *posi, *prev;
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

pathmap *phm_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size) {
    int length = sizeof (struct phm_nod *) * size;
    //
    pathmap *pmap = (pathmap *) malloc(sizeof (pathmap));
    if(!pmap) return NULL;
    memset(pmap, '\0', sizeof (pathmap));
    pmap->gethash = keyfunc;
    pmap->compare = compare;
    pmap->hashsize = size;
    pmap->count = 0;
    //
    pmap->hmaplist = malloc(length);
    if (NULL == pmap->hmaplist) {
        free(pmap);
        return NULL;
    }
    memset(pmap->hmaplist, '\0', length);
    //
    return pmap;
}

void phm_dele(pathmap *pmap) {
    unsigned int inde;
    struct phm_nod *posi;
    //
    if (NULL == pmap) return;
    for (inde = 0; inde < pmap->hashsize; inde++)
        for (posi = pmap->hmaplist[inde]; NULL != posi; posi = pnod_del(posi));
    //
    free(pmap->hmaplist);
    free(pmap);
}

void phm_insert(void *key, void *data, pathmap *pmap) {
    unsigned int inde = hashinde(key, pmap);
    struct phm_nod *root = pmap->hmaplist[inde];
    //
    if (0 != vlist_update(root, key, data, pmap->compare)) { //(1)
        vlist_append(&(pmap->hmaplist[inde]), key, data);
        pmap->count++;
    }
}

void phm_remove(void *key, pathmap *pmap) {
    unsigned int inde = hashinde(key, pmap);
    if (0 == vlist_remove(&(pmap->hmaplist[inde]), key, pmap->compare)) {
        pmap->count--;
    }
}

void *phm_value(void *key, pathmap *pmap) {
    struct phm_nod *posi;
    unsigned int inde = hashinde(key, pmap);
    for (posi = pmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
        if (pmap->compare(key, posi->key)) {
            return (posi->data);
        }
    }
    return NULL;
}

void phm_for_each_do(pathmap *pmap, int (call_back) (void *, void *)) {
    unsigned int inde = 0;
    struct phm_nod *posi;
    for (inde = 0; inde < pmap->hashsize; inde++) {
        for (posi = pmap->hmaplist[inde]; NULL != posi; posi = posi->next) {
            call_back(posi->key, posi->data);
        }
    }
}

inline int phm_count(pathmap *pmap) {
    return pmap->count;
}

