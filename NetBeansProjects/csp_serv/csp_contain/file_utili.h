/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef FILE_UTILI_H
#define FILE_UTILI_H

#include "virtu_path.h"

#ifdef __cplusplus
extern "C" {
#endif

//
char *get_appli_path();
#define CREAT_CONF_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/conf/%s", get_appli_path(), FILE_NAME);
#define CREAT_LOGS_PATH(PATH_NAME, FILE_NAME) \
    sprintf(PATH_NAME, "%s/logs/%s", get_appli_path(), FILE_NAME);

//
// #define CSPAGE_FUNNAME_DEFAULT "csp_main"
int dyli_open_vlist(struct vpa_list *vlist);
void dyli_close_vlist(struct vpa_list *vlist);


#ifdef __cplusplus
}
#endif

#endif /* FILE_UTILI_H */

