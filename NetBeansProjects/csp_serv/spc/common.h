
#ifndef COMMON_H
#define	COMMON_H

#ifdef	__cplusplus
extern "C" {
#endif

#define LINE_TXT                1024
#define TEXT_LENGTH             0x2000 // 8K 
#define SECTION_LENGTH          0x4000 // 16K        
#define CLANG_TEXT_SIZE         0x4000 // 16K
#define MAX_PATH                1024
#define STRING_LENGTH           512
#define FILE_NAME_LEN           512
#define COAND_LENGTH            32

#define mkzero(TEXT)            TEXT[0] = '\0'
    
//
#define MAX_PARAM_CNT   256
    
#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_H */

