/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "file_utili.h"
#include "parsing.h"

//
#define DEFAULT_VALUE    "Default"
#define ANCH_INDE_DEFAULT "~/anchor_inde.ndx"
#define ANCH_DATA_DEFAULT "~/anchor_data.acd"

//
static char *kill_blank(char *stri) {
    char *toke;
    for (toke = stri; ' ' == toke[0]; ++toke) {
        if ('\0' == toke[0]) {
            return NULL;
        }
    }
    return toke;
}

//anchor index=~/anchor_inde.ndx data=~/anchor_data.acd
//authen host=127.0.0.1:3306 db=AuthDB user=root passwd=root
// backup max_modify=16 interv_modi=600 max_delete=128 interv_dele=300
static dav_str_t inde_tok = dav_string("index");
static dav_str_t data_tok = dav_string("data");
static dav_str_t anchor_tok = dav_string("anchor");
//
static dav_str_t host_tok = dav_string("host");
static dav_str_t db_tok = dav_string("db");
static dav_str_t user_tok = dav_string("user");
static dav_str_t pwd_tok = dav_string("passwd");
static dav_str_t authen_tok = dav_string("authen");
//
static dav_str_t backup_tok = dav_string("backup");
static dav_str_t max_mod_tok = dav_string("max_modify");
static dav_str_t inte_mod_tok = dav_string("interv_modi");
static dav_str_t max_dele_tok = dav_string("max_delete");
static dav_str_t inte_dele_tok = dav_string("interv_dele");

//
//
int bakctrl_parser(bakup_config *bakup_ctrl, char *line_txt) {
    char *toksp = kill_blank(line_txt + backup_tok.len);
    if(!toksp) return -1;
    //
    char *tokep = NULL;
    do {
        tokep = strchr(toksp, ' ');
        if(tokep) tokep[0] = '\0';
        //
        if(!strncasecmp((char *)max_mod_tok.data, toksp, max_mod_tok.len)) {
            bakup_ctrl->max_modify = atoi(toksp+max_mod_tok.len+0x01);
        } else if(!strncasecmp((char *)inte_mod_tok.data, toksp, inte_mod_tok.len)) {
            bakup_ctrl->modi_interv = atoi(toksp+inte_mod_tok.len+0x01);
        } else if(!strncasecmp((char *)max_dele_tok.data, toksp, max_dele_tok.len)) {
            bakup_ctrl->max_delete = atoi(toksp+max_dele_tok.len+0x01);
        } else if(!strncasecmp((char *)inte_dele_tok.data, toksp, inte_dele_tok.len)) {
            bakup_ctrl->dele_interv = atoi(toksp+inte_dele_tok.len+0x01);
        }
        if(tokep) toksp = kill_blank(tokep + 0x01);
    } while(tokep);
//
    return 0x00;
}

//
static void saddr_split(serv_addr *saddr, const char *addr_txt) {
    char *toke = NULL;
    memset(saddr, '\0', sizeof (serv_addr));
    toke = strchr(addr_txt, ':');
    if (toke) {
        strncpy(saddr->sin_addr, addr_txt, toke - addr_txt);
        saddr->sin_port = atoi(++toke);
    } else strcpy(saddr->sin_addr, addr_txt);
}

//
int anchor_parser(anchor_files *afiles, char *line_txt) {
    char *toksp = kill_blank(line_txt + anchor_tok.len);
    if(!toksp) return -1;
    //
    char *tokep = NULL;
    do {
        tokep = strchr(toksp, ' ');
        if(tokep) tokep[0] = '\0';
        //
        if(!strncasecmp((char *)inde_tok.data, toksp, inde_tok.len)) {
            strcpy(afiles->anch_idx, toksp+inde_tok.len+0x01);
            if (!strcasecmp(DEFAULT_VALUE, (char *) toksp+inde_tok.len+0x01)) {
                strcpy(afiles->anch_idx, ANCH_INDE_DEFAULT);
            } else strcpy(afiles->anch_idx, toksp+inde_tok.len+0x01);
        } else if(!strncasecmp((char *)data_tok.data, toksp, data_tok.len)) {
            if (!strcasecmp(DEFAULT_VALUE, (char *) toksp+inde_tok.len+0x01)) {
                strcpy(afiles->anch_acd, ANCH_DATA_DEFAULT);
            } else strcpy(afiles->anch_acd, toksp+data_tok.len+0x01);
        }
        if(tokep) toksp = kill_blank(tokep + 0x01);
    } while(tokep);
//
    return 0x00;
}

int authen_parser(struct auth_config *config, char *line_txt) {
    char *toksp = kill_blank(line_txt + anchor_tok.len);
    if(!toksp) return -1;
    //
    char *tokep = NULL;
    do {
        tokep = strchr(toksp, ' ');
        if(tokep) tokep[0] = '\0';
        //
        if(!strncasecmp((char *)host_tok.data, toksp, host_tok.len)) {
            saddr_split(&config->address, toksp+host_tok.len+0x01);
        } else if(!strncasecmp((char *)db_tok.data, toksp, db_tok.len)) {
            strcpy(config->database, toksp+db_tok.len+0x01);
        } else if(!strncasecmp((char *)user_tok.data, toksp, user_tok.len)) {
            strcpy(config->user_name, toksp+user_tok.len+0x01);
        } else if(!strncasecmp((char *)pwd_tok.data, toksp, pwd_tok.len)) {
            strcpy(config->password, toksp+pwd_tok.len+0x01);
        }
        if(tokep) toksp = kill_blank(tokep + 0x01);
    } while(tokep);
//
    return 0x00;
}

//-1:error 0x00:comment 0x01:anchor 0x02:authen
int parse_line_type(char *line_txt) {
    if(!line_txt) return -1;
    if('#' == line_txt[0]) return 0x00;
    //
    char *toke = kill_blank(line_txt);
    if(!toke) return -1;
    if(!strncasecmp((char *)anchor_tok.data, toke, anchor_tok.len)) return 0x01;
    else if(!strncasecmp((char *)authen_tok.data, toke, authen_tok.len)) return 0x02;
    else if(!strncasecmp((char *)backup_tok.data, toke, backup_tok.len)) return 0x03;
    //
    return -1;
}

void fill_line_end(char *line_txt) {
    char *toke = strpbrk(line_txt, "\r\n");
    if(toke) toke[0] = '\0';
}

//
int parse_config(anchor_files *afiles, struct auth_config *config, bakup_config *bakup_ctrl) {
    char dav_cloud[1024];
    CREAT_CONF_PATH(dav_cloud, DAV_CLOUD_DEFAULT)
    //
    FILE *conf_fp = NULL;
    conf_fp = fopen64(dav_cloud, "rb");
    if (!conf_fp) {
        debug_logger_s("open config file error! %s\n", dav_cloud);
        return -1;
    }
    //
    char line_txt[1024];
    int line_type;
    while (fgets(line_txt, 1024, conf_fp)) {
        line_type = parse_line_type(line_txt);
        fill_line_end(line_txt);
        if (0x01 == line_type) {
            if (anchor_parser(afiles, line_txt)) {
                return -1;
                fclose(conf_fp);
            }
        } else if (0x02 == line_type) {
            if (authen_parser(config, line_txt)) {
                return -1;
                fclose(conf_fp);
            }
        } else if (0x03 == line_type) {
            if (bakctrl_parser(bakup_ctrl, line_txt)) {
                return -1;
                fclose(conf_fp);
            }
        }
    }
//
    fclose(conf_fp);
    return 0x00;
}