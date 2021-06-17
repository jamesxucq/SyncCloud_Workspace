/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "dav_macro.h"
#include "stri_utili.h"

//
char *lsprif(char *dest, const char *format, ...) {
    va_list arg;
    int done;
    //
    if (!dest) return NULL;
    va_start(arg, format);
    done = vsprintf(dest, format, arg);
    va_end(arg);
    if (0x00 > done) return NULL;
    //
    return dest + done;
}

char *lscpy(char * dest, const char * src) {
    if (!dest) return NULL;
    register char *d = dest;
    register const char *s = src;
    while (*d = *s++) d++;
    return d;
}

char *bakup_folder(time_t bak_time) { // Nov_15_1995_GMT
    static char folder[TIME_LENGTH];
    struct tm *utctm;
    //
    if (!bak_time) bak_time = time(NULL);
    utctm = gmtime(&bak_time);
    if (!strftime(folder, TIME_LENGTH, "%b_%d_%Y_%Z", utctm)) return NULL;
    //
    return folder;
}


