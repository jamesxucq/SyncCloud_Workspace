/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>

#include "stri_utili.h"
#include "file_utili.h"
#include "logger.h"

#include "anchor.h"

// #pragma pack(1)
struct idxent {
    uint32 anchor_number;
    uint64 data_oset;
    time_t time_stamp;
};
// #pragma pack()
struct fatent {
    unsigned int action_type; /* add mod del list recu */
    char file_name[MAX_PATH * 3];
    unsigned int reserved; /* reserved */
};

//
anchor_files _anchor_files_;

//
#define FAIL_EXIT(DATA, RENO) { \
    debug_logger("anchor file error.\n"); \
    fclose(DATA); return RENO; }

uint32 load_last_anchor(char *location) {
    FILE *anchor_idxfp = NULL;
    char anch_idx[MAX_PATH * 3];
    struct idxent last_inde;
    //
    memset(&last_inde, '\0', sizeof (struct idxent));
    APPEND_PATH(anch_idx, location, _anchor_files_.anch_idx);
    anchor_idxfp = fopen64(anch_idx, "ab+");
    if (!anchor_idxfp) {
        debug_logger_ds("open file error. %d %s\n", errno, anch_idx);
        return 0;
    }
    //
    uint64 idx_size;
    FILE_SIZE_STREAM(idx_size, anchor_idxfp)
    if (0 < idx_size) {
        if (fseeko64(anchor_idxfp, -(__off64_t)sizeof (struct idxent), SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
            if (0 >= fread(&last_inde, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
    }
    fclose(anchor_idxfp);
    //
    return last_inde.anchor_number;
}

uint32 addi_anchor(char *location) {
    FILE *anchor_idxfp = NULL;
    char idxdat[MAX_PATH * 3];
    struct idxent ins_inde, last_inde;
    //
debug_logger("addi_anchor\n");
    memset(&ins_inde, '\0', sizeof (struct idxent));
    memset(&last_inde, '\0', sizeof (struct idxent));
    APPEND_PATH(idxdat, location, _anchor_files_.anch_idx);
    anchor_idxfp = fopen64(idxdat, "ab+");
    if (!anchor_idxfp) {
        debug_logger_ds("open file error. %d %s\n", errno, idxdat);
        return 0;
    }
    //
    uint64 idx_size;
    FILE_SIZE_STREAM(idx_size, anchor_idxfp)
    if (0 < idx_size) {
        if (fseeko64(anchor_idxfp, -(__off64_t)sizeof (struct idxent), SEEK_END)) FAIL_EXIT(anchor_idxfp, 0)
        if (0 >= fread(&last_inde, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
    }
    ins_inde.anchor_number = last_inde.anchor_number + 1;
    APPEND_PATH(idxdat, location, _anchor_files_.anch_acd);
    FILE_SIZE(ins_inde.data_oset, idxdat)
    ins_inde.time_stamp = time(NULL);
    //
    if (fseeko64(anchor_idxfp, 0, SEEK_END)) FAIL_EXIT(anchor_idxfp, 0) {
        if (0 >= fwrite(&ins_inde, sizeof (struct idxent), 1, anchor_idxfp)) FAIL_EXIT(anchor_idxfp, 0)
            // _LOG_DEBUG("add new anchor:%d file offset:%d", ins_inde.anchor_number, ins_inde.data_oset); // disable by james 20130409
    }
    fclose(anchor_idxfp);
    return ins_inde.anchor_number;
}

//
#define OPERA_FSDO(FSDO, AOPE) { \
    FSDO.action_type = AOPE->action_type; \
    strcpy(FSDO.file_name, AOPE->file_name); \
    FSDO.reserved = AOPE->reserved; \
}

int flush_action(char *location, struct openod *opesdo) {
    struct fatent faen;
    // if NULL == action_list ,means not change
debug_logger("flush_action\n");
    char anch_acd[MAX_PATH * 3];
    APPEND_PATH(anch_acd, location, _anchor_files_.anch_acd);
    FILE *anchor_datfp = fopen64(anch_acd, "ab+");
    if (!anchor_datfp) {
        debug_logger_ds("open file error. %d %s\n", errno, anch_acd);
        return -1;
    }
    if (fseeko64(anchor_datfp, 0, SEEK_END)) FAIL_EXIT(anchor_datfp, -1)
    //
    OPERA_FSDO(faen, opesdo)
    if (0 >= fwrite(&faen, sizeof (struct fatent), 1, anchor_datfp))
         FAIL_EXIT(anchor_datfp, -1)
    fclose(anchor_datfp);
    return 0;
}