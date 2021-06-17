
#ifndef BUILDER_H
#define	BUILDER_H

#include "middle.h"

#ifdef	__cplusplus
extern "C" {
#endif

#include "inte.h"

    int build_clang_page(struct built_list *midd_list, struct clang_line *clist, unsigned int req_ivk);
    int build_clang_obje(struct built_list *midd_list, struct clang_line *clist);

#ifdef	__cplusplus
}
#endif

#endif	/* BUILDER_H */

