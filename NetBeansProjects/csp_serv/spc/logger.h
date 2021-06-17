/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

//
    typedef enum {
        log_lvl_fatal = 0,
        log_lvl_error,
        log_lvl_warn,
        log_lvl_info,
        //
        log_lvl_debug,
        log_lvl_trace,
        log_lvl_max
    } log_lvl_t;

//
void set_parse_file(char *exist_file, char *file_name);
void log_printf(log_lvl_t log_lvl, const char *fmt, ...);

//
#define _LOG_PARSE_SET(exist_file, new_file) set_parse_file(exist_file, new_file)

#define _LOG_FATAL(fmt, ...) log_printf(log_lvl_fatal, ":"fmt"\n", ##__VA_ARGS__)
#define _LOG_ERROR(fmt, ...) log_printf(log_lvl_error, ":"fmt"\n", ##__VA_ARGS__)
#define _LOG_WARN(fmt, ...) log_printf(log_lvl_warn, ":"fmt"\n", ##__VA_ARGS__)
#define _LOG_INFO(fmt, ...) log_printf(log_lvl_info, ":"fmt"\n", ##__VA_ARGS__)
#define _LOG_DEBUG(fmt, ...) log_printf(log_lvl_debug, ":"fmt"\n", ##__VA_ARGS__)
#define _LOG_TRACE(fmt, ...) log_printf(log_lvl_trace, ":"fmt"\n", ##__VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */

