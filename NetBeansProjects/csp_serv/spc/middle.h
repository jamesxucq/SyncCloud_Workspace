
/* 
 * File:   middle.h
 * Author: Administrator
 *
 * Created on 2016.12.6, PM 2:38
 */

#ifndef MIDDLE_H
#define MIDDLE_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//
struct built_list {
    char *clan_file[MAX_PARAM_CNT];
    char **last_clan;
};

//
void set_middle_file(char *midd_name);
void open_middle_file(struct built_list *midd_list);
int flush_midd_file(struct built_list *midd_list);
void dele_midd_list(struct built_list *midd_list);
int put_midd_file(struct built_list *midd_list, char *file_name);

#ifdef __cplusplus
}
#endif

#endif /* MIDDLE_H */

