#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "logger.h"

//
void debug_logger(const char *str1) {
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, str1, strlen(str1));
        close(log_fd);
    }
}

void debug_logger_s(const char *fmt, const char *str1) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, str1);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}

void debug_logger_ss(const char *fmt, const char *str1, const char *str2) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, str1, str2);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}

void debug_logger_d(const char *fmt, int int1) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, int1);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}

void debug_logger_dd(const char *fmt, int int1, int int2) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, int1, int2);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}

void debug_logger_sd(const char *fmt, const char *str1, int int1) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, str1, int1);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}

void debug_logger_ds(const char *fmt, int int1, const char *str1) {
    char buffer[1024];
    int length = snprintf(buffer, 1024 - 1, fmt, int1, str1);
    int log_fd = open64(DAV_CLOUD_LOG_DEFAULT, O_WRONLY | O_APPEND | O_CREAT, 0666);
    if(-1 != log_fd) {
        write(log_fd, buffer, length);
        close(log_fd);
    }
}