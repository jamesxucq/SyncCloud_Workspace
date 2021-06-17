/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef MAP_PATH_H
#define MAP_PATH_H

#include "path_hmap.h"
#include "virtu_path.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MATCH_BACKET_SIZE     0x80000  // 512K
    
//

unsigned int lh_strhash(void *src);
int equal_str(void *k1, void *k2);

//
pathmap *build_path_hmap(struct vpa_list *vlist);
void destory_path_hmap(pathmap *vpmap);

#ifdef __cplusplus
}
#endif

#endif /* MAP_PATH_H */

