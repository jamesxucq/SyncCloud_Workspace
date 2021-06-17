
#ifndef PARSER_H
#define	PARSER_H

#ifdef	__cplusplus
extern "C" {
#endif  
    
#include"common.h"
#include"line_type.h"
#include"conv.h"

struct inte_line *create_clang_virtual(struct inte_line *list, char *vtual_txt, unsigned int vtual_type);
struct inte_line *build_clang_start(struct inte_line *list, char *vtual_txt1, char *vtual_txt2);
struct inte_line *build_clang_end(struct inte_line *list);
struct inte_line *find_require_undo(struct inte_line *parse_list);
int parse_cspla_data(struct inte_line *parse_list, char *cspla_data);

#ifdef	__cplusplus
}
#endif

#endif	/* PARSER_H */

