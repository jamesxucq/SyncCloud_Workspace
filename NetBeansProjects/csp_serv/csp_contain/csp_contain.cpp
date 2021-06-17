/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "fcgi_stdio.h"
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
//
#include "contain_macro.h"
#include "map_path.h"
#include "virtu_path.h"
#include "file_utili.h"
#include "parse_conf.h"
#include "base_pool.h"
#include "memo_pool.h"

/*
 */
typedef void *(*CSPINV)(MYSQL *sql_sock, tiny_pool_t *tpool);
//
static MYSQL *sql_conn;
static int seconds;

MYSQL_PING_HANDER(sql_conn, seconds)

//
int main(void) {
    char xml_config[MAX_PATH * 3];
    CREAT_CONF_PATH(xml_config, CSP_CONTAIN_DEFAULT)
            struct cont_config *config = load_config_bzl(xml_config);
    access_inital(config->access_log.logfile);
    log_inital(config->runtime_log.logfile, config->runtime_log.debug_level);
    //
    tiny_pool_t *tpool = NULL;
    if (config->tpool.avail) {
        _LOG_DEBUG("create memory pool!");
        tpool = memo_pool_creat(&config->tpool);
        if (!tpool) return (EXIT_FAILURE);
    }
    MYSQL *sql_sock = NULL;
    if (config->bpool.avail) {
        _LOG_DEBUG("connect to database!");
        sql_sock = base_pool_creat(&config->bpool);
        if (!sql_sock) return (EXIT_FAILURE);
    }
    sql_conn = sql_sock;
    seconds = config->bpool.recon_timeout;
    CHECK_BASE_POOL(config->bpool.avail, sql_sock, seconds)
            //
    if (dyli_open_vlist(config->vlist)) return (EXIT_FAILURE);
    pathmap *vp_map = build_path_hmap(config->vlist);
    if (!vp_map) return (EXIT_FAILURE);
    //
    char *req_uri;
    while (FCGI_Accept() >= 0) {
        req_uri = getenv("REQUEST_URI");
        // _LOG_DEBUG("req_uri:|%s|", req_uri);
        CSPINV csp_invoke = (CSPINV)phm_value(req_uri, vp_map);
        //
        CHECK_MEMO_POOL(config->tpool.avail, tpool, config->tpool.reset_rate)
        // _LOG_DEBUG("invoke csp_main. csp_main|%08x|", csp_main);
        csp_invoke(sql_sock, tpool);
        // _LOG_DEBUG("end invoke, continue!");
    }
    //
    destory_path_hmap(vp_map);
    dyli_close_vlist(config->vlist);
    if (config->bpool.avail) close_base_pool(sql_sock);
    if (config->tpool.avail) close_memo_pool(tpool);
    log_final();
    access_final();
    return (EXIT_SUCCESS);
}

