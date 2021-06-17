/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include "virtu_path.h"

#define VLIST_PREPEND(head, add) { \
    add->next = head; \
    head = add; \
}

struct vpa_list *vlist_newvpa(vpa_lsth vlsth) {
    struct vpa_list *vlistmp = (struct vpa_list *) malloc(sizeof (struct vpa_list));
    if (!vlistmp) return NULL;
    memset(vlistmp, '\0', sizeof (struct vpa_list));
    //
    VLIST_PREPEND(*vlsth, vlistmp);
    return vlistmp;
}

void dele_vlist_path(struct vpa_list *vlist) {
    struct vpa_list *vlistmp = NULL, *del_vlist = NULL;
    vlistmp = vlist;
    while (vlistmp) {
        del_vlist = vlistmp;
        vlistmp = vlistmp->next;
        if (del_vlist) {
            free(del_vlist->lib_path);
            del_vlist->lib_path = NULL;
            free(del_vlist->csp_name);
            del_vlist->csp_name = NULL;
        }
    }
}

void dele_vlist(struct vpa_list *vlist) {
    struct vpa_list *vlistmp = NULL, *del_vlist = NULL;
    vlistmp = vlist;
    while (vlistmp) {
        del_vlist = vlistmp;
        vlistmp = vlistmp->next;
        if (del_vlist) {
            if (del_vlist->lib_path) free(del_vlist->lib_path);
            if (del_vlist->csp_name) free(del_vlist->csp_name);
            if (del_vlist->vir_path) free(del_vlist->vir_path);
            free(del_vlist);
        }
    }
}