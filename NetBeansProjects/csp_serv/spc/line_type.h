
#ifndef SEMT_TYPE_H
#define	SEMT_TYPE_H

#ifdef	__cplusplus
extern "C" {
#endif

//
#define CLANG_START_TOKEN       "<%"       // <%
#define CLANG_END_TOKEN         "%>"       // %>
#define CLANG_TOKLEN            0x02
#define EQUAL_TOKLEN            0x02
#define COMMENT_TOKEN           "--"
#define COMMENT_TOKLEN          0x02
#define MIME_CONTENT_TYPE       "Content-Type"

//
#define LINE_UNKNOWN_TYPE       0
#define SECTION_UNKNOWN_TYPE    LINE_UNKNOWN_TYPE
#define LINE_COMMENT_TYPE       0x01000000      // //
#define LINE_CTRL_TYPE          0x02000000      // @ control // pagesize autoflush mimeheader require include
#define CTRL_PAGE_SIZE                  0x00000001 // pagesize
#define CTRL_AUTO_FLUSH                 0x00000002 // autoflush
#define CTRL_MIME_HEADER                0x00000004 // mimeheader
#define CTRL_INCLUDE                    0x00000008 // include
#define CTRL_INCLUDE_ONCE               0x00000010 // include once
#define CTRL_REQUIRE                    0x00000020 // require
#define CTRL_REQUIRE_DONE               0x00000040 // require done // virtual
    //////////////////////////////////////////
#define LINE_FUNCTION_TYPE      0x04000000      // $ function // echo print
#define FUNC_ECHO                       0x00000001 // echo
#define FUNC_PRINT                      0x00000002 // print
    //////////////////////////////////////////
#define LINE_EQUAL_TYPE         0x08000000      // = equ string =$ equ dec =% equ undefine
#define EQUAL_STRING                    0x00000001 // = equ string
#define EQUAL_DECIMAL                   0x00000002 // =$ equ dec
#define EQUAL_UNDEFINE                  0x00000004 // =% equ undefine
    //////////////////////////////////////////
#define LINE_HTML_TYPE          0x10000000      // html
#define SECTION_HTML_TYPE       LINE_HTML_TYPE      // html
    //////////////////////////////////////////
#define LINE_CLANG_TYPE         0x20000000      // c language    
#define CLANG_LINE                      0x00000001 // clang
#define CLANG_CFILE                     0x00000002 // clang file output // virtual
#define CLANG_START                     0x00000004 // clang function start // virtual
#define CLANG_END                       0x00000008 // clang function end // virtual
#define SECTION_CLANG_TYPE      LINE_CLANG_TYPE      // c language
    //////////////////////////////////////////    // virtual
#define LINE_HIDE_TYPE          0x40000000      // hide line
#define SECTION_HIDE_TYPE       LINE_HIDE_TYPE      // hide line
    //////////////////////////////////////////  // setion attribut
#define CLANG_HEAD                      0x00010000
#define CLANG_BODY                      0x00020000
#define CLANG_EXTEND                    0x00040000

#define STYPE_CODE(TYPE)    (TYPE&0xFF000000)
#define SATTR_CODE(TYPE)    (TYPE&0x00FF0000)
#define STION_CODE(TYPE)    (TYPE&0xFFFF0000)
#define ENTRY_CODE(TYPE)    (TYPE&0x0000FFFF)

#define SET_REQUIRE_DONE(REQ) REQ->type = STION_CODE(REQ->type) | CTRL_REQUIRE_DONE;

    ////////////////////////////////////////
    unsigned int get_control_type(char *ctrl_txt);
    unsigned int get_function_type(char *func_txt);
    unsigned int get_equal_type(char *equal_txt);

    //////////////////////////////////////////
#define COAND_PAGE_SIZE                  "@pagesize"
#define COAND_AUTO_FLUSH                 "@autoflush"
#define COAND_MIME_HEADER                "@mimeheader"
#define COAND_INCLUDE                    "@include"
#define COAND_INCLUDE_ONCE               "@include_once"
#define COAND_REQUIRE                    "@require" 

    //////////////////////////////////////////
#define COAND_ECHO                       "$echo"
#define COAND_PRINT                      "$print"

    //////////////////////////////////////////  

#ifdef	__cplusplus
}
#endif

#endif	/* SEMT_TYPE_H */

