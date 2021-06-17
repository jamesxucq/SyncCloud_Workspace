
/* 
 * File:   compile.h
 * Author: Administrator
 *
 * Created on 2016.12.8, AM 10:47
 */

#ifndef COMPILE_H
#define COMPILE_H

#include "middle.h"

#ifdef __cplusplus
extern "C" {
#endif

//

struct spc_param {
    int function;
    char *cspa_file[MAX_PARAM_CNT];
};
    
//
int handle_compile(struct built_list *midd_list, struct spc_param *spc_para);

#ifdef __cplusplus
}
#endif

#endif /* COMPILE_H */

