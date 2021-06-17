/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include "encoding.h"
#include "logger.h"
#include "auth_utili.h"
#include "dav_stru.h"

//
#define QUERY_LENGTH    512
#define LARGE_QUERY_LENGTH    2048

//
struct auth_config _auth_config_;

//
static void caddr_split(serv_addr *caddr, const char *addr_txt) {
    char *toke = NULL;
    memset(caddr, '\0', sizeof (serv_addr));
    toke = strchr(addr_txt, ':');
    if (toke) {
        strncpy(caddr->sin_addr, addr_txt, toke - addr_txt);
        caddr->sin_port = atoi(++toke);
    } else strcpy(caddr->sin_addr, addr_txt);
}

//
#define SELECT_ACCESS_QUERY \
	"SELECT User_id, Pool_size, Access_key, Auth_host FROM user WHERE User_name = \"%s\";"
static int get_accval_by_uname(struct acce_value *accval, struct auth_config *config, const char *user_name) {
    // MYSQL mysql;
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
debug_logger_ds("couldn't connect to engine!%d %s\n", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_ACCESS_QUERY, user_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
debug_logger_s("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
debug_logger_s("couldn't get result from %s\n", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo));
    if (0x01 != mysql_num_rows(respo)) {
debug_logger("the result set error\n");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while (row = mysql_fetch_row(respo)) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    accval->uid = atol(row[inde]);
                    break;
                case 1:
                    accval->pool_size = atoi(row[inde]);
                    break;
                case 2:
                    strcpy(accval->access_key, row[inde]);
                    break;
                case 3:
                    caddr_split(&accval->caddr, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

//
int query_access_bzl(struct acce_value *accval, const char *user_name) {
debug_logger("query_access_bzl\n");
    if(get_accval_by_uname(accval, &_auth_config_, user_name)) {
        debug_logger_s("query access error user_name:%s\n", user_name);
        return -1;
    }    
    trans_auth_chks(accval->_author_chks_, accval->uid, accval->access_key);
    return 0x00;
}

//
#define SELECT_AUTHEN_QUERY \
	"SELECT  User_id, Password, Salt FROM user WHERE User_name = \"%s\";"
static int get_autval_by_uname(struct auth_value *autval, struct auth_config *config, const char *user_name) {
    // MYSQL mysql;
    MYSQL *sql_sock;
    MYSQL_RES *respo;
    MYSQL_ROW row;
    char query[QUERY_LENGTH];
//
    sql_sock = mysql_init(NULL);
    if (!mysql_real_connect(sql_sock, config->address.sin_addr, config->user_name,
            config->password, config->database, config->address.sin_port, NULL, 0)) {
debug_logger_ds("couldn't connect to engine!%d %s\n", mysql_errno(sql_sock), mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
//
    sprintf(query, SELECT_AUTHEN_QUERY, user_name);
    if (mysql_real_query(sql_sock, query, (unsigned int) strlen(query))) {
debug_logger_s("query failed (%s)\n", mysql_error(sql_sock));
        mysql_close(sql_sock);
        return -1;
    }
    if (!(respo = mysql_store_result(sql_sock))) {
debug_logger_s("couldn't get result from %s\n", mysql_error(sql_sock));
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
    // _LOG_INFO("number of fields returned: %d", mysql_num_fields(respo));
    if (0x01 != mysql_num_rows(respo)) {
debug_logger("the result set error\n");
        mysql_free_result(respo);
        mysql_close(sql_sock);
        return -1;
    }
//
    while (row = mysql_fetch_row(respo)) {
        int inde;
        for (inde = 0; inde < mysql_num_fields(respo); inde++) {
            // _LOG_INFO("%s ", row[inde]);
            switch (inde) {
                case 0:
                    autval->uid = atol(row[inde]);
                    break;
                case 1:
                    strcpy(autval->password, row[inde]);
                    break;
                case 2:
                    strcpy(autval->_salt_, row[inde]);
                    break;
            }
        }
        // _LOG_INFO("query auth data base ok!"); // disable by james 20130409
    }
    mysql_free_result(respo);
    mysql_close(sql_sock);
//
    return 0;
}

int query_authen_bzl(struct auth_value *autval, const char *user_name) {
    if(get_autval_by_uname(autval, &_auth_config_, user_name)) {
        debug_logger_s("query authen error user_name:%s\n", user_name);
        return -1;
    }
    decode_base64_text(autval->password, autval->password, autval->_salt_);
    return 0x00;
}