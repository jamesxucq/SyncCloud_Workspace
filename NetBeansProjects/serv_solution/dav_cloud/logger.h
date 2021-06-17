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

#define DAV_CLOUD_LOG_DEFAULT    "/var/log/sync_cloud/dav_cloud.log"
    
void debug_logger(const char *str1);
void debug_logger_s(const char *fmt, const char *str1);
void debug_logger_ss(const char *fmt, const char *str1, const char *str2);
void debug_logger_sd(const char *fmt, const char *str1, int int1);
void debug_logger_ds(const char *fmt, int int1, const char *str1);
void debug_logger_d(const char *fmt, int int1);
void debug_logger_dd(const char *fmt, int int1, int int2);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */

