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
#include <unistd.h>
#include <errno.h>

#include "logger.h"
#include "stri_utili.h"
#include "dav_macro.h"
#include "capacity.h"

#define FAIL_EXIT(DATFD, RENO) { \
    debug_logger("capacity file error.\n"); \
    close(DATFD); return RENO; }

int reset_capacity(char *location, struct openod *opesdo) {
    uint64 incre_size = 0, bakup_size = 0;
//
debug_logger("reset capacity\n");
    switch (opesdo->action_type) {
    case ADDI:
        incre_size += opesdo->file_size;
        break;
    case MODIFY:
        incre_size += (opesdo->file_size - opesdo->reserved);
        break;
    case DELE:
        incre_size -= opesdo->file_size;
        bakup_size += opesdo->file_size;
        break;
    case COPY:
        incre_size += opesdo->file_size;
        break;
    }
    // _LOG_DEBUG("inc_size:%ul bakup_size:%ul ", inc_size>>30, bakup_size>>30); // disable by james 20130409
    char capa_data[MAX_PATH * 3];
    struct capacity capa;
    memset(&capa, '\0', sizeof (struct capacity));
    APPEND_PATH(capa_data, location, CAPACITY_DEFAULT);
    int capa_fide = open64(capa_data, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == capa_fide) {
        debug_logger_ds("open file error. %d %s\n", errno, capa_data);
        return -1;
    }
    if (-1 == lseek64(capa_fide, 0, SEEK_SET)) FAIL_EXIT(capa_fide, -1)
    if (-1 == read(capa_fide, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fide, -1)
    // _LOG_DEBUG("read capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    capa.used_size += incre_size;
    capa.bakup_size += bakup_size;
    if (-1 == lseek64(capa_fide, 0, SEEK_SET)) FAIL_EXIT(capa_fide, -1)
    if (-1 == write(capa_fide, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fide, -1)
    // _LOG_DEBUG("capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    close(capa_fide);
    return 0;
}

// 0:ok -1:error 0x01:exception
int check_capacity(char *location, uint32 pool_size) {
    uint64 large_pool = ((uint64)pool_size) << 30;
    // _LOG_DEBUG("inc_size:%ul bakup_size:%ul ", inc_size>>30, bakup_size>>30); // disable by james 20130409
debug_logger("check capacity\n");
    char capa_data[MAX_PATH * 3];
    struct capacity capa;
    memset(&capa, '\0', sizeof (struct capacity));
    APPEND_PATH(capa_data, location, CAPACITY_DEFAULT);
    int capa_fide = open64(capa_data, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (-1 == capa_fide) {
        debug_logger_ds("open file error. %d %s\n", errno, capa_data);
        return -1;
    }
    if (-1 == lseek64(capa_fide, 0, SEEK_SET)) FAIL_EXIT(capa_fide, -1)
    if (-1 == read(capa_fide, &capa, sizeof (struct capacity))) FAIL_EXIT(capa_fide, -1)
    // _LOG_DEBUG("read capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    if((capa.used_size + capa.bakup_size) >= large_pool) {
        debug_logger_s("pool is full : %s\n", capa_data);
        close(capa_fide);
        return -1;
    }
    // _LOG_DEBUG("capa.used_size:%ul capa.bakup_size:%ul", capa.used_size>>30, capa.bakup_size>>30); // disable by james 20130409
    close(capa_fide);
    return 0;
}
