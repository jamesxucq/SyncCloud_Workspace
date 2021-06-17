#ifndef _QINTERNAL_H
#define _QINTERNAL_H

/*
 * Internal Macros
 */
#ifdef BUILD_DEBUG
#define DEBUG(fmt, args...) fprintf(stderr, "[DEBUG] " fmt " (%s:%d)\n", ##args, __FILE__, __LINE__);
#else
#define DEBUG(fms, args...)
#endif  /* BUILD_DEBUG */

/*
 * Macro Functions
 */
#define CONST_STRLEN(x)     (sizeof(x) - 1)

#define DYNAMIC_VSPRINTF(s, f)                                          \
    do {                                                                \
        size_t _strsize;                                                \
        for(_strsize = 1024; ; _strsize *= 2) {                         \
            s = (char*)malloc(_strsize);                                \
            if(s == NULL) {                                             \
                DEBUG("DYNAMIC_VSPRINTF(): can't allocate memory.");    \
                break;                                                  \
            }                                                           \
            va_list _arglist;                                           \
            va_start(_arglist, f);                                      \
            int _n = vsnprintf(s, _strsize, f, _arglist);               \
            va_end(_arglist);                                           \
            if(_n >= 0 && _n < _strsize) break;                         \
            free(s);                                                    \
        }                                                               \
    } while(0)

#define STATIC_VSPRINTF(s, l, f)    \
    va_list _arglist;               \
    va_start(_arglist, f);          \
    vsnprintf(s, l, f, _arglist);   \
    va_end(_arglist);

/*
 * Internal Definitions
 */
#define CRLF "\r\n"
#define MAX_LINEBUF (1023+1)
#define DEF_DIR_MODE  (S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define DEF_FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

/*
 * qInternalCommon.c
 */
extern char _q_x2c(char hex_up, char hex_low);
extern char *_q_makeword(char *str, char stop);
extern char *_q_makeword_s(char *word, char *str, char stop);
#define URL_ENCODE_LEN(size) ((size * 3) + 1)
extern char *_q_urlencode(char *enstr, const void *bin, size_t size);
extern size_t _q_urldecode(char *str);
extern char *_q_fgets(char *str, size_t size, FILE *fp);
extern char *_q_fgetline(FILE *fp, size_t initsize);
extern int _q_unlink(const char *pathname);
extern char *_q_strcpy(char *dst, size_t size, const char *src);
extern char *_q_strtrim(char *str);
extern char *_q_strunchar(char *str, char head, char tail);
extern char *_q_filename(const char *filepath);
extern off_t _q_filesize(const char *filepath);
extern off_t _q_iosend(FILE *outfp, FILE *infp, off_t nbytes);
extern int _q_countread(const char *filepath);
extern bool _q_countsave(const char *filepath, int number);

#endif  /* _QINTERNAL_H */
