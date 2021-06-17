/* 
 * File:   astyle.h
 * Author: Administrator
 *
 * Created on 2016.12.8, AM 10:02
 */

#ifndef ASTYLE_H
#define ASTYLE_H

#include "middle.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_astyle_enable(int enable);
int format_clang_style(struct built_list *midd_list);


#ifdef __cplusplus
}
#endif

#endif /* ASTYLE_H */

