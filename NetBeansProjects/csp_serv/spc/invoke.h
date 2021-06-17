
#ifndef INVOKE_H
#define	INVOKE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include"common.h"
#include"inte.h"

    struct funivk {
        unsigned int invoke;
        char line_txt[LINE_TXT];
        struct funivk *sibling, *children;
    };

#define FIVK_NULL       0x00 // require file
#define FIVK_CONBUFF    0x00000001 // have line text buffer
#define FIVK_REQUIRE    0x00000002 // client require
#define FIVK_CONSQL     0x00000004 // have sql connection
#define FIVK_TPOOL      0x00000008 // have memory pool
#define FIVK_SESSION    0x00000100 // session declare
#define FIVK_CONTYPE    0x00000200 // undef content type
#define FIVK_AFLUSH     0x00000400 // undef auto flush
#define FIVK_QPARSE     0x00000800 // disable qparse
#define FIVK_PARAM      0x01000000 // require file argument

#define INHERIT_FIVK_TYPE(TYPE) (0x00FFFFFF & TYPE)
    // int valid_line_buffer(struct clang_line *clist);

    struct funivk *create_invoke(struct inte_line *plist);
    struct funivk *find_vtree(struct funivk *fivk, char *line_txt);
    void destory_invoke(struct funivk *fivk);

#ifdef	__cplusplus
}
#endif

#endif	/* INVOKE_H */

