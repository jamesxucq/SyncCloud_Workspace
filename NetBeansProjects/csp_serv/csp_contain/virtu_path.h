/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#ifndef VIRTU_PATH_H
#define VIRTU_PATH_H

#include "contain_macro.h"

#ifdef __cplusplus
extern "C" {
#endif

    struct dyli_fun {
        void *handle;
        void *funptr;
    };
    
    struct vpa_list {
        struct dyli_fun dlfup;
        char *lib_path;
        char *csp_name;
        char *vir_path;
        struct vpa_list *next;
    };
    typedef struct vpa_list** vpa_lsth;

    //
    struct vpa_list *vlist_newvpa(vpa_lsth vlsth);
    void dele_vlist_path(struct vpa_list *vlist);
    void dele_vlist(struct vpa_list *vlist);

#ifdef __cplusplus
}
#endif

#endif /* VIRTU_PATH_H */

