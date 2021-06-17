/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include "logger.h"

#define BUFFER_SIZE 1024
static char *LEVEL_TIPS[] = {"[FATAL]", "[ERROR]", "[WARN]", "[INFO]", "[DEBUG]", "[TRACE]", "[MAX]"};
// static int TIPS_LEN[] = {0x07, 0x07, 0x06, 0x06, 0x07, 0x07, 0x05};
static char _parse_file_[260];
void set_parse_file(char *exist_file, char *file_name) {
    if(exist_file) strcpy(exist_file, _parse_file_);
    strcpy(_parse_file_, file_name);
// printf("parse_file:%s\n", parse_file);
}

void log_printf(log_lvl_t log_lvl, const char *fmt, ...) {
    char buffer[1024];
    int prev_len;
    prev_len = sprintf(buffer, "spc:%s:%s", LEVEL_TIPS[log_lvl], _parse_file_);
    //
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer + prev_len, BUFFER_SIZE - prev_len - 1, fmt, ap);
    va_end(ap);
    //
    fprintf(stderr, buffer);
}