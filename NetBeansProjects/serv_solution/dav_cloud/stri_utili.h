/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef STRI_UTILI_H
#define STRI_UTILI_H

#ifdef __cplusplus
extern "C" {
#endif

    //
char *lsprif(char *dest, const char *format, ...);
char *lscpy(char * dest, const char * src);
    
//
#define TEXT_SPLIT(line_txt, str, delim, tokep) { \
    char *toksp; \
    if(str) { \
        toksp = str; \
        tokep = strstr(toksp, delim); \
        if (tokep) *tokep = '\0'; \
        line_txt = toksp; \
    } else line_txt = NULL; \
}

#define SPLIT_TOKEN(line_txt, delim, tokep) { \
    char *toksp; \
    if(tokep) { \
        toksp = tokep + strlen(delim); \
        tokep = strstr(toksp, delim); \
        if (tokep) *tokep = '\0'; \
        line_txt = toksp; \
    } else line_txt = NULL; \
}

#define SPLIT_VALUE(tval, line_txt) { \
    char *toksp, *tokep; \
    toksp = strchr(line_txt, ' '); \
    if (toksp) *toksp = '\0'; \
    ++toksp; \
    tokep = strrchr(toksp, ' '); \
    if (tokep) *tokep = '\0'; \
    tval = toksp; \
}

#define SPLIT_LINE(tval, line_txt) { \
    char *toke; \
    toke = strstr(line_txt, ": "); \
    if (toke) *toke = '\0'; \
    tval = toke + 2; \
}   
    
//
#define APPEND_PATH(APATH, PATH, SUBDIR) \
        sprintf(APATH, "%s%s", PATH, SUBDIR + 1)
#define POOL_PATH(PPATH, PATH, SUBDIR) \
        sprintf(PPATH, "%s/user_pool%s", PATH, SUBDIR)

//
    char *bakup_folder(time_t bak_time);
#define LAST_BAKUP(BAKPATH, PATH, SUBDIR, BAKTIME) \
         sprintf(BAKPATH, "%s/backup/%s%s_%x", PATH, bakup_folder(BAKTIME), strrchr(SUBDIR, '/'), (uint32)BAKTIME)
#define BAKUP_PATH(BAKPATH, PATH, BAKTIME, SUBDIR) \
        sprintf(BAKPATH, "%s/backup/%s%s", PATH, bakup_folder(BAKTIME), SUBDIR + 1)

#ifdef __cplusplus
}
#endif

#endif /* STRI_UTILI_H */

