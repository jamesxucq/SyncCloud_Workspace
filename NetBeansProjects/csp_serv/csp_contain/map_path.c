/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "file_utili.h"
#include "map_path.h"
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
pathmap *build_path_hmap(struct vpa_list *vlist) {
    pathmap *vpmap = phm_create(lh_strhash, equal_str, MATCH_BACKET_SIZE);
    if(!vpmap) {
        _LOG_FATAL("build_path_hmap phm_create error.");
        return NULL;
    }
    //
    struct vpa_list *vlistmp = vlist;
    while (vlistmp) {
// _LOG_DEBUG("csp_name|%s| lib_path|%s| vir_path|%s| handle|%08x| funptr|%08x|", vlistmp->csp_name, vlistmp->lib_path, vlistmp->vir_path, vlistmp->dlfup.handle, vlistmp->dlfup.funptr);
        phm_insert(vlistmp->vir_path, vlistmp->dlfup.funptr, vpmap);
        vlistmp = vlistmp->next;
    }
    dele_vlist_path(vlist);
    //
    return vpmap;
}

void destory_path_hmap(pathmap *vpmap) {
    phm_dele(vpmap);
}