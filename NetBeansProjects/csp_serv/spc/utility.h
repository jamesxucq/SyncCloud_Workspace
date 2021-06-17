
#ifndef UTILITY_H
#define UTILITY_H

#ifdef __cplusplus
extern "C" {
#endif

    //
    const char *strlchr(const char *str);
    const char *strlast(const char *str);
    char *strzcpy(char *dest, const char *source, size_t count);
    //
    char *lsprif(char *dest, const char *format, ...);
    char *lscpy(char * dest, const char * src);
    char *lsncpy(char *dest, const char *src, size_t n);
    char *lscat(char * dest, const char * src);
    char *lsncat(char *dest, const char *src, size_t n);
    int spc_file_exis(char *spc_file[]);

    ////////////////////////////////////////
    int is_cspla(char *str);
    int clang_path(char *lang, char *str);
    char *repl_str(char *str);
    // char *file_name(char *filename, char *filepath);
    void *cfile_load(const char *filepath, size_t *nbytes);
    size_t cfile_save(const char *filepath, const void *buf, size_t size);

    ////////////////////////////////////////
    char *ctrl_page_size(char *ctrl_str, char *line_txt);
    int ctrl_auto_flush(char *line_txt);
    int ctrl_mime_header(char *ctrl_str, char *line_txt);
    char *ctrl_include(char *ctrl_str, char *line_txt);
    char *ctrl_include_once(char *ctrl_str, char *ctrl_str1, char *line_txt);
    char *ctrl_require_file(char *ctrl_str, char *line_txt);
    char *ctrl_require(char *ctrl_str, char *ctrl_str1, char *line_txt);
    char *func_echo(char *func_str, char *func_str1, char *line_txt);
    char *func_print(char *func_str, char *func_str1, char *line_txt);
    char *equal_string(char *equal_str, char *line_txt);
    char *equal_decimal(char *equal_str, char *line_txt);
    char *equal_undefine(char *equal_str, char *func_str1, char *line_txt);
    ////////////////////////////////////////
    char *search_section(char *stion_txt, unsigned int *stion_type, char *parse_text);
    char *next_line(char *line_txt, unsigned int *line_type, char *stion_text);
    unsigned int check_valid(char *line_txt);
    void display_lineno(char *cspla_data, char *parse_posi);
    //
    int check_decla_name(char *rline_txt, char *sline_txt);
    char *find_decal(char *func_name, char *decal_str);

#ifdef __cplusplus
}
#endif

#endif /* UTILITY_H */

