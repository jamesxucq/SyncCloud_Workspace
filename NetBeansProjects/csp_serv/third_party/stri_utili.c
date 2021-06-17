#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "stri_utili.h"

//

const char *strlchr(const char *str) {
    const char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;
    //
    for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
        if (*char_ptr == '\0')
            return (char_ptr - 1);
    }
    //
    longword_ptr = (unsigned long int *) char_ptr;
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof (longword) > 4) {
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    if (sizeof (longword) > 8) abort();
    //
    for (;;) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & ~longword & himagic) != 0) {
            const char *cp = (const char *) (longword_ptr - 1);
            //
            if (cp[0] == 0)
                return (cp - 1);
            if (cp[1] == 0)
                return cp;
            if (cp[2] == 0)
                return (cp + 1);
            if (cp[3] == 0)
                return (cp + 2);
            if (sizeof (longword) > 4) {
                if (cp[4] == 0)
                    return (cp + 3);
                if (cp[5] == 0)
                    return (cp + 4);
                if (cp[6] == 0)
                    return (cp + 5);
                if (cp[7] == 0)
                    return (cp + 6);
            }
        }
    }
}

const char *strlast(const char *str) {
    const char *char_ptr;
    const unsigned long int *longword_ptr;
    unsigned long int longword, himagic, lomagic;
    //
    for (char_ptr = str; ((unsigned long int) char_ptr & (sizeof (longword) - 1)) != 0; ++char_ptr) {
        if (*char_ptr == '\0')
            return char_ptr;
    }
    //
    longword_ptr = (unsigned long int *) char_ptr;
    himagic = 0x80808080L;
    lomagic = 0x01010101L;
    if (sizeof (longword) > 4) {
        himagic = ((himagic << 16) << 16) | himagic;
        lomagic = ((lomagic << 16) << 16) | lomagic;
    }
    if (sizeof (longword) > 8) abort();
    //
    for (;;) {
        longword = *longword_ptr++;
        if (((longword - lomagic) & ~longword & himagic) != 0) {
            const char *cp = (const char *) (longword_ptr - 1);
            //
            if (cp[0] == 0)
                return cp;
            if (cp[1] == 0)
                return (cp + 1);
            if (cp[2] == 0)
                return (cp + 2);
            if (cp[3] == 0)
                return (cp + 3);
            if (sizeof (longword) > 4) {
                if (cp[4] == 0)
                    return (cp + 4);
                if (cp[5] == 0)
                    return (cp + 5);
                if (cp[6] == 0)
                    return (cp + 6);
                if (cp[7] == 0)
                    return (cp + 7);
            }
        }
    }
}

//

char *strzcpy(char *dest, const char *source, size_t count) {
    char *start = dest;
    while (count && (*dest++ = *source++)) count--; /* copy string */
    if (count) /* pad out with zeroes */
        while (--count) *dest++ = '\0';
    else *dest = '\0';
    return (start);
}

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
    while ((*d = *s++)) d++;
    return d;
}

char *lsncpy(char *dest, const char *src, size_t n) {
    char c;
    //
    if (!dest) return NULL;
    if (n >= 4) {
        size_t n4 = n >> 2;
        for (;;) {
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            c = *src++;
            *dest = c;
            if (c == '\0') break;
            dest++;
            if (--n4 == 0) goto last_chars;
        }
        return dest;
    }
last_chars:
    n &= 3;
    if (n == 0) {
        *dest = '\0';
        return dest;
    }
    for (;;) {
        c = *src++;
        *dest = c;
        if (c == '\0') break;
        dest++;
        if (--n == 0) {
            *dest = '\0';
            return dest;
        }
    }
    //
    return dest;
}

char *lscat(char * dest, const char * src) {
    if (!dest) return NULL;
    return lscpy(dest + strlen(dest), src);
}

char *lsncat(char *dest, const char *src, size_t n) {
    if (!dest) return NULL;
    char *destptr = dest + strlen(dest);
    for (; n > 0 && (*destptr = *src) != '\0'; src++, destptr++, n--);
    if (n == 0) *destptr = '\0';
    return destptr;
}
