

/* 
 * File:   utility.h
 * Author: Administrator
 *
 * Created on 2016.12.6, AM 11:03
 */

#ifndef UTILI_H
#define UTILI_H

#ifdef __cplusplus
extern "C" {
#endif

//
#define MAX_PATH        260
#define mkzero(TEXT)            TEXT[0] = '\0'

//
int fill_middle(char *midd_file[], char *file_name);
int fill_object(char *obj_file[], char *clan_file);
int pcc_file_exis(char *pcc_file[]);
void clear_middle_file(char *middle_file[]);
void clear_cache(char *cache_file);
    

#ifdef __cplusplus
}
#endif

#endif /* UTILI_H */

