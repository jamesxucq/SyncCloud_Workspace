#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>

#include"common.h"
#include "logger.h"
#include"line_type.h"
#include"utility.h"

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

//

int clang_path(char *lang, char *str) {
    strcpy(lang, str);
    char *suffix = strrchr(lang, '.');
    if (!suffix) return -1;
    //
    if (!strcasecmp(suffix, ".csp")) {
        *(++suffix) = 'c';
        *(++suffix) = 'p';
        *(++suffix) = 'p';
        *(++suffix) = '\0';
    }
    //
    return 0;
}

int is_cspla(char *str) {
    char *suffix = strrchr(str, '.');
    if (!suffix) return -1;
    if (strcasecmp(suffix, ".csp"))
        return -1;
    return 0;
}

#define ESCA_SIZE     2 // escape

char *repl_str(char *str) {
    char restr[TEXT_LENGTH];
    char *ss, *sd;
    // printf("repl_str1:|%s|\n", str);    
    strcpy(restr, str);
    for (ss = restr, sd = str; '\0' != *ss; ss++, sd++) {
        switch (*ss) {
            case '\t':
                memcpy(sd, "\\t", ESCA_SIZE);
                sd++;
                break;
            case '\r':
                memcpy(sd, "\\r", ESCA_SIZE);
                sd++;
                break;
            case '\n':
                memcpy(sd, "\\n", ESCA_SIZE);
                sd++;
                break;
            case '"':
                memcpy(sd, "\\\"", ESCA_SIZE);
                sd++;
                break;
            default:
                *sd = *ss;
                break;
        }
    }
    *sd = '\0';
    // printf("repl_str2:|%s|\n", str);   
    return str;
}

void *cfile_load(const char *filepath, size_t *nbytes) {
    int fd;
    if ((fd = open(filepath, O_RDONLY, 0)) < 0) return NULL;
    //
    struct stat fs;
    if (fstat(fd, &fs) < 0) {
        close(fd);
        return NULL;
    }
    //
    size_t size = fs.st_size;
    if (nbytes != NULL && *nbytes > 0 && *nbytes < fs.st_size) size = *nbytes;
    //
    void *buf = malloc(size + 1);
    if (buf == NULL) {
        close(fd);
        return NULL;
    }
    //
    ssize_t count = read(fd, buf, size);
    close(fd);
    //
    if (count != size) {
        free(buf);
        return NULL;
    }
    //
    ((char *) buf)[count] = '\0';
    //
    if (nbytes != NULL) *nbytes = count;
    return buf;
}

size_t cfile_save(const char *filepath, const void *buffer, size_t size) {
    FILE *fp = fopen(filepath, "wb");
    if (!fp) return -1;
    size_t count = fwrite(buffer, 1, size, fp);
    fclose(fp);
    return count;
}

////////////////////////////////////////

char *next_str(char *str, char *line_txt) {
    char *ss, *se;
    if (!line_txt || '\0' == line_txt[0]) return NULL;
    //
    ss = strpbrk(line_txt, "\t ,=");
    for (; '\t' == *ss || ' ' == *ss || ',' == *ss || '=' == *ss || '"' == *ss || '(' == *ss; ss++);
    se = strpbrk(ss, "\t ,=\r\n");
    if (se) {
        if (ss == se) se = NULL;
        else strzcpy(str, ss, se - ss);
    } else strcpy(str, ss);
    //
    for (ss = str; '\0' != *ss; ss++);
    ss--;
    if ('"' == *ss || ')' == *ss) *ss = '\0';
    //
    return se;
}

char *ctrl_page_size(char *ctrl_str, char *line_txt) {
    char str[STRING_LENGTH];
    char *se;

    next_str(str, line_txt);
    for (se = str; '\0' != *se; se++);
    se--;
    if ('K' == *se || 'k' == *se) {
        *se = '\0';
        sprintf(ctrl_str, "%d", 1024 * atoi(str));
    } else strcpy(ctrl_str, "PAGE_BUFFER_SIZE");

    return ctrl_str;
}

int ctrl_auto_flush(char *line_txt) {
    char str[STRING_LENGTH];
    int auto_flush = 0;
    //
    next_str(str, line_txt);
    if (strcasecmp("true", str)) auto_flush = 1;
    //
    return auto_flush;
}

int ctrl_mime_header(char *ctrl_str, char *line_txt) {
    next_str(ctrl_str, line_txt);
    return strstr(line_txt, MIME_CONTENT_TYPE) ? 0x01 : 0x00;
}

char *next_strex(char *str, char *line_txt) {
    char *ss, *se;
    if (!line_txt || '\0' == line_txt[0]) return NULL;
    //
    ss = strpbrk(line_txt, "\t ,=");
    for (; '\t' == *ss || ' ' == *ss || ',' == *ss || '=' == *ss || '(' == *ss; ss++);
    se = strpbrk(ss, "\t ,=\r\n");
    if (se) {
        if (ss == se) se = NULL;
        else strzcpy(str, ss, se - ss);
    } else strcpy(str, ss);
    //
    for (ss = str; '\0' != *ss; ss++);
    ss--;
    if (')' == *ss) *ss = '\0';
    //
    return se;
}

static void _strupr_(char *dest, char *src) {
    for (; '\0' != src[0]; src++, dest++) {
        dest[0] = src[0];
        if ('a' <= dest[0] && 'z' >= dest[0])
            dest[0] ^= 0x20;
    }
}

char *inclu_once_str(char *incl_str, char *name_str) {
    strcpy(incl_str, name_str);
    char *tok;
    for (tok = incl_str; (tok = strpbrk(tok, "/. \"")); tok++) *tok = '_';
    _strupr_(incl_str, incl_str);
    return incl_str;
}

char *ctrl_include(char *ctrl_str, char *line_txt) {
    next_strex(ctrl_str, line_txt);
    return ctrl_str;
}

char *ctrl_include_once(char *ctrl_str, char *ctrl_str1, char *line_txt) {
    next_strex(ctrl_str, line_txt);
    inclu_once_str(ctrl_str1, ctrl_str);
    return ctrl_str;
}

char *ctrl_require_file(char *ctrl_str, char *line_txt) {
    next_str(ctrl_str, line_txt);
    return ctrl_str;
}

char *ctrl_require(char *ctrl_str, char *ctrl_str1, char *line_txt) {
    char str[STRING_LENGTH];
    //
    char *se = next_str(str, line_txt);
    char *tok = strrchr(str, '/');
    if (tok) strcpy(ctrl_str, ++tok);
    else strcpy(ctrl_str, str);
    tok = strrchr(ctrl_str, '.');
    if (tok) *tok = '_';
    ////////////////////////////////////////
    if (ctrl_str1) {
        if (next_str(str, se)) strcpy(ctrl_str1, str);
        else strcpy(ctrl_str1, "NULL");
    }
    //
    return ctrl_str;
}

static char *next_fstr(char *str, char *line_txt) {
    char *ss, *se;
    if (!line_txt || '\0' == line_txt[0]) return NULL;
    ////////////////////////////////////////////////////////
    ss = strpbrk(line_txt, "\t ,()");
    if (!ss) return NULL;
    for (; '\t' == *ss || ' ' == *ss || ',' == *ss || '(' == *ss || ')' == *ss; ss++);
    if ('"' == *ss) se = strchr(ss + 0x01, '"') + 0x01;
    else se = strpbrk(ss, "\t ,();\r\n");
    //
    if (se) {
        if (ss == se) se = NULL;
        else strzcpy(str, ss, se - ss);
    } else strcpy(str, ss);
    //////////////////////////////////
    // printf("str:|%s| line_txt:|%s| se:|%s|\n", str, line_txt, se);
    return se;
}

char *func_echo(char *func_str, char *func_str1, char *line_txt) {
    char str[STRING_LENGTH];
    char *se;
    //
    se = next_fstr(str, line_txt);
    strcpy(func_str, str);
    mkzero(func_str1);
    while ((se = next_fstr(str, se))) {
        func_str1 = lscpy(func_str1, ", ");
        func_str1 = lscpy(func_str1, str);
        // printf("str:|%s| se:|%s|\n", str, se);
    }
    // printf("func_str:|%s| func_str1:|%s|\n", func_str, func_str1);
    return func_str;
}

char *func_print(char *func_str, char *func_str1, char *line_txt) {
    char str[STRING_LENGTH];
    char *se;
    //
    se = next_fstr(str, line_txt);
    strcpy(func_str, str);
    mkzero(func_str1);
    while ((se = next_fstr(str, se))) {
        func_str1 = lscpy(func_str1, ", ");
        func_str1 = lscpy(func_str1, str);
    }
    // printf("func_str:|%s| func_str1:|%s|\n", func_str, func_str1);
    return func_str;
}

char *equal_string(char *equal_str, char *line_txt) {
    strcpy(equal_str, line_txt + 1);
    return equal_str;
}

char *equal_decimal(char *equal_str, char *line_txt) {
    strcpy(equal_str, line_txt + EQUAL_TOKLEN);
    return equal_str;
}

char *equal_undefine(char *equal_str, char *equal_str1, char *line_txt) {
    char str[STRING_LENGTH];
    char *se = next_strex(str, line_txt);
    strcpy(equal_str, str);
    if (se) strcpy(equal_str1, se);
    return equal_str;
}

////////////////////////////////////////
// <%e-- --e%>
#define ERROR_SECTION     -1
#define INVALID_SECTION     0x01

unsigned int check_valid(char *line_txt) {
    char *str = line_txt;
    // comment line
    if (!strncmp(COMMENT_TOKEN, str, COMMENT_TOKLEN)) {
        str = strstr(COMMENT_TOKLEN + str, COMMENT_TOKEN);
        if (str) {
            str += COMMENT_TOKLEN;
            if ('\0' == *str) return 0x00;
            _LOG_FATAL("comment toke error!");
            return ERROR_SECTION;
        } else {
            _LOG_FATAL("comment toke error!");
            return ERROR_SECTION;
        }
    }
    // all space char   
    for (; ' ' == *str || '\t' == *str || '\r' == *str || '\n' == *str; str++);
    if ('\0' == *str) return INVALID_SECTION;
    //
    return 0x00;
}

int check_number_valid(char *line_txt, int number) {
    char *str;
    int index;
    for (index = number, str = line_txt; index && (' ' == *str || '\t' == *str || '\r' == *str || '\n' == *str); index--, str++);
    if ('\0' == *str) return 1;
    //
    return 0;
}

char *search_section(char *stion_txt, unsigned int *stion_type, char *parse_text) {
    char *ss, *se;
    // printf("------------|%s|\n", parse_text); // for test
    if (!parse_text || '\0' == parse_text[0]) return NULL;
    ////////////////////////////////////////////////////////
    ss = strstr(parse_text, CLANG_START_TOKEN);
    if (parse_text == ss) {
        ss += CLANG_TOKLEN;
        se = strstr(ss, CLANG_END_TOKEN);
        // printf("ss:|%s|\n", ss);
        if (!se) {
            _LOG_FATAL("csp file toke error!");
            return NULL;
        }
        if ('h' == *ss || 'H' == *ss) {
            if ('h' != *(se - 0x01) && 'H' != *(se - 0x01)) {
                _LOG_FATAL("head toke error!");
                return NULL;
            }
            strzcpy(stion_txt, ss + 0x01, se - ss - 0x02);
            *stion_type = SECTION_CLANG_TYPE | CLANG_HEAD;
        } else if ('e' == *ss || 'E' == *ss) {
            // printf("e:|%s|\n", ss);
            if ('e' != *(se - 0x01) && 'E' != *(se - 0x01)) {
                _LOG_FATAL("head toke error!");
                return NULL;
            }
            strzcpy(stion_txt, ss + 0x01, se - ss - 0x02);
            *stion_type = SECTION_CLANG_TYPE | CLANG_EXTEND;
        } else {
            strzcpy(stion_txt, ss, se - ss);
            *stion_type = SECTION_CLANG_TYPE | CLANG_BODY;
        }
        se += CLANG_TOKLEN;
    } else {
        if (ss) {
            se = ss;
            strzcpy(stion_txt, parse_text, se - parse_text);
        } else {
            se = parse_text + strlen(parse_text);
            strcpy(stion_txt, parse_text);
        }
        *stion_type = SECTION_HTML_TYPE;
    }
    ////////////////////////////////////////////////////////
    return se;
}

char *line_coand_end(char *line_txt) {
    char *str;
    int find_ok = 1, invalid = 0;
    //
    str = line_txt;
    while (find_ok) {
        str = strpbrk(str, "\t \",=");
        if (str) {
            for (; '\t' == *str || ' ' == *str || '(' == *str || ')' == *str; str++);
            switch (*str) {
                case '"':
                    for (str++; '"' != *str; str++) if ('\\' == *str) str++;
                    str++;
                    break;
                case ',':
                    str++;
                    for (; '\t' == *str || ' ' == *str || '(' == *str || ')' == *str; str++);
                    break;
                case '=':
                    str++;
                    for (; '\t' == *str || ' ' == *str || '(' == *str || ')' == *str; str++);
                    invalid++;
                    break;
                default:
                    if (invalid) find_ok--;
                    else invalid++;
                    break;
            }
        } else {
            for (str = line_txt; '\0' != *str; str++);
            find_ok--;
        }
    }
    //
    return str;
}

char *line_ctrl_end(char *line_txt) {
    char *str = strpbrk(line_txt, ";\r\n");
    if (str) for (; ';' == *str || '\r' == *str || '\n' == *str; str++);
    else for (str = line_txt; '\0' != *str; str++);
    return str;
}

char *line_hide_end(char *line_txt) {
    char *str;
    //
    str = line_txt;
    if ('#' == *line_txt) {
        str = strstr(str, "\n");
        if (str) str++;
        else _LOG_ERROR("single comment error!");
    }
    //
    return str;
}

char *line_comm_end(char *line_txt) {
    char *str;
    //
    str = line_txt + 1;
    if (('/' == *str) || ('#' == *line_txt)) {
        str = strstr(str, "\n");
        if (str) str++;
        else _LOG_ERROR("single comment error!");
    } else if ('*' == *str) {
        str = strstr(str, "*/");
        if (str) str += 2;
        else _LOG_ERROR("multi comment error!");
    }
    //
    return str;
}

char *spbrk_ex(char *str) {
    int loop = 0, first = 0;
    char *islang = NULL, *tok;
    // printf("spbrk_ex|%s|\n", str);
    tok = str;
    do {
        for (; ' ' == *tok || '\t' == *tok || '\r' == *tok || '\n' == *tok; tok++);
        if ('\0' != *tok) {
            first++;
            switch (tok[0]) {
                case '"':
                    for (tok++; '"' != *tok; tok++) if ('\\' == *tok) tok++;
                    tok++;
                    break;
                case '#':
                case '$':
                case '@':
                    loop++;
                    break;
                case '{':
                case '}':
                case ';':
                    islang = tok++;
                    first = 0;
                    break;
                case '=':
                    if (1 == first) loop++;
                    else tok++;
                    break;
                case '/':
                    if ('/' == *(tok + 1) || '*' == *(tok + 1)) loop++;
                    break;
                default:
                    tok++;
                    break;
            }
        } else {
            tok = NULL;
            loop++;
        }
        if (loop && islang) tok = islang;
    } while (!loop);
    //
    return tok;
}

char *next_line(char *line_txt, unsigned int *line_type, char *clcon) {
    char *ss, *se;
    //
    if (!clcon || '\0' == clcon[0]) return NULL;
    ////////////////////////////////////////////////////////
    ss = spbrk_ex(clcon);
    if (ss) {
        // printf("ss:|%s|\n", ss);
        switch (ss[0]) {
            case '#':
                se = line_hide_end(ss);
                *line_type = LINE_HIDE_TYPE;
            case '/':
                se = line_comm_end(ss);
                strzcpy(line_txt, ss, se - ss);
                *line_type = LINE_COMMENT_TYPE;
                break;
            case '@':
                se = line_ctrl_end(ss);
                strzcpy(line_txt, ss, se - ss);
                *line_type = LINE_CTRL_TYPE;
                break;
            case '$':
                se = line_coand_end(ss);
                strzcpy(line_txt, ss, se - ss);
                *line_type = LINE_FUNCTION_TYPE;
                break;
            case '=':
                se = line_coand_end(ss);
                strzcpy(line_txt, ss, se - ss);
                *line_type = LINE_EQUAL_TYPE;
                break;
            case '{':
            case '}':
            case ';':
                se = ss + 1;
                strzcpy(line_txt, clcon, se - clcon);
                *line_type = LINE_CLANG_TYPE;
                break;
            case '\0':
                se = ss;
                strcpy(line_txt, clcon);
                *line_type = LINE_CLANG_TYPE;
                break;
        }
    } else se = ss;
    ////////////////////////////////////////////////////////
    return se;
}

void display_lineno(char *cspla_data, char *parse_posi) {
    int line_numb = 0x00;
    char *parse_toke = cspla_data;
    while ((parse_toke = strchr(parse_toke + 0x01, '\n'))) {
        line_numb++;
        if (parse_toke > parse_posi) break;
    }
    _LOG_INFO("line: %d", line_numb);
}

//

int spc_file_exis(char *spc_file[]) {
    int chk_val = 0x00;
    //
    int inde;
    for (inde = 0x00; spc_file[inde]; inde++) {
        if (access(spc_file[inde], F_OK)) {
            if (!chk_val) {
                printf("build clang failed!\n");
                chk_val++;
            }
            printf("%s ", spc_file[inde]);
        }
    }
    if (chk_val) printf("not exists.\n");
    //
    return chk_val;
}

//

static char *kill_blank(char *stri) {
    char *toke;
    for (toke = stri; ' ' == toke[0]; ++toke) {
        if ('\0' == toke[0]) {
            return NULL;
        }
    }
    return toke;
}

static char *find_invoke(char *func_name, char *invo_str) {
    char *toksp = strchr(invo_str, '=');
    if (!toksp) return NULL;
    toksp = kill_blank(0x01 + toksp);
    if (!toksp) return NULL;
    char *tokep = strpbrk(toksp, " (");
    if (!tokep) return NULL;
    //
    strzcpy(func_name, toksp, tokep - toksp);
    return func_name;
}

char *find_decal(char *func_name, char *decal_str) {
    char *toksp = kill_blank(decal_str);
    if (!toksp) return NULL;
    toksp = strchr(0x01 + toksp, ' ');
    if (!toksp) return NULL;
    toksp = kill_blank(0x01 + toksp);
    if (!toksp) return NULL;
    // for char *func();
    if ('*' == toksp[0x00]) {
        toksp = kill_blank(++toksp);
        if (!toksp) return NULL;
    }
    char *tokep = strpbrk(toksp, " (");
    if (!tokep) return NULL;
    //
    strzcpy(func_name, toksp, tokep - toksp);
    return func_name;
}

int check_decla_name(char *rline_txt, char *sline_txt) {
    char invok[MAX_PATH];
    char decal[MAX_PATH];
// printf("rline_txt:|%s| sline_txt:|%s|", rline_txt, sline_txt);
    find_invoke(invok, rline_txt);
    find_decal(decal, sline_txt);
// printf("invok:|%s| decal:|%s|", invok, decal);
    return strcmp(invok, decal);
}