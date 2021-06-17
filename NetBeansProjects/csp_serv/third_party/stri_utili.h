/* 
 * File:   stri_utili.h
 * Author: Administrator
 *
 * Created on 2016.11.28, PM 4:00
 */

#ifndef STRI_UTILI_H
#define STRI_UTILI_H

#ifdef __cplusplus
extern "C" {
#endif

    const char *strlchr(const char *str);
    const char *strlast(const char *str);
    char *strzcpy(char *dest, const char *source, size_t count);
    //
    char *lsprif(char *dest, const char *format, ...);
    char *lscpy(char * dest, const char * src);
    char *lsncpy(char *dest, const char *src, size_t n);
    char *lscat(char * dest, const char * src);
    char *lsncat(char *dest, const char *src, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* STRI_UTILI_H */

