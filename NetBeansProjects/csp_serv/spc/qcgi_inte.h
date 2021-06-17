/* 
 * File:   qcgi_inte.h
 * Author: Administrator
 *
 * Created on 2016.12.10, PM 3:11
 */

#ifndef QCGI_INTE_H
#define QCGI_INTE_H

#ifdef __cplusplus
extern "C" {
#endif

//
int init_qcgi_hmap();
void fina_qcig_hmap();
unsigned int check_qcig_invoke(char *str);


#ifdef __cplusplus
}
#endif

#endif /* QCGI_INTE_H */

