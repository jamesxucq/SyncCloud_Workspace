/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <mysql.h>

#include "logger.h"
#include "base_pool.h"

//
MYSQL *base_pool_creat(struct base_pool *bpool) {
    MYSQL *sql_sock;
    char enable = 0x01;
//
    sql_sock = mysql_init(NULL);
    mysql_options(sql_sock, MYSQL_OPT_RECONNECT, (char *)&enable);
    if (!mysql_real_connect(sql_sock, bpool->address.sin_addr, bpool->user_name,
            bpool->password, bpool->database, bpool->address.sin_port, NULL, 0)) {
_LOG_WARN("couldn't connect to engine!%d %s", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return NULL;
    }
//
    return sql_sock;
}

void close_base_pool(MYSQL *sql_sock) {
     mysql_close(sql_sock);
}
   