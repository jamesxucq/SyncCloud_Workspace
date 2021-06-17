/* 
 * File:   path_hmap.h
 * Author: Administrator
 */

#ifndef PATH_HMAP_H
#define	PATH_HMAP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <string.h>

struct phm_nod {
    void *key;
    void *data;
    struct phm_nod *next;
};

struct _pathmap_ {
    unsigned int (*gethash)(void *);
    int (*compare)(void *, void *);
    unsigned int hashsize;
    unsigned int count;
    struct phm_nod **hmaplist;
};

typedef struct _pathmap_ pathmap;

pathmap *phm_create(unsigned int (*keyfunc)(void *), int (*compare)(void *, void *), unsigned int size);
void phm_dele(pathmap *pmap);
void phm_insert(void *key, void *data, pathmap *pmap);
void phm_remove(void *key, pathmap *pmap);
void *phm_value(void *key, pathmap *pmap);
void phm_for_each_do(pathmap *pmap, int (call_back) (void *, void *));
int phm_count(pathmap *pmap);

#ifdef	__cplusplus
}
#endif

#endif	/* PATH_HMAP_H */

