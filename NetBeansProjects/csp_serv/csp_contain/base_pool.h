/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef BASE_POOL_H
#define BASE_POOL_H

#include <unistd.h>
#include <signal.h>

// #include "logger.h"
#include "parse_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

MYSQL *base_pool_creat(struct base_pool *bpool);
void close_base_pool(MYSQL *sql_sock);

// mysql_ping(MYSQL *mysql)
#define MYSQL_PING_HANDER(sql_sock, seconds) \
    void sigalrm_fn(int sig) { \
        mysql_ping(sql_sock); \
        _LOG_WARN("mysql connect timeout, ping!"); \
        alarm(seconds); \
    }

#define CHECK_BASE_POOL(avail, sql_sock, seconds) \
    if(avail) { \
        signal(SIGALRM, sigalrm_fn); \
        alarm(seconds); \
    }

#ifdef __cplusplus
}
#endif

#endif /* BASE_POOL_H */

