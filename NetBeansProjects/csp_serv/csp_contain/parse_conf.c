/* parse_conf.c
 * Created on: 2010-3-8
 * Author: Divad
 */
#include <stdlib.h>
#include <string.h>
#include <libxml/parser.h>

#include "error_code.h"
#include "file_utili.h"
#include "virtu_path.h"
#include "parse_conf.h"

//
#define DEFAULT_VALUE    "Default"

//
struct cont_config _cont_config_;
char _config_path_[MAX_PATH * 3];

//

int memory_pool_parser(struct memo_pool *tpool, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    if (NULL != curNode) {
        sValue = xmlGetProp(curNode, (const xmlChar *) "Avail");
        if (!strcasecmp("Enable", (char *) sValue)) tpool->avail = 0x01;
        else if (!strcasecmp("Disable", (char *) sValue)) tpool->avail = 0x00;
        xmlFree(sValue);
        //
        sValue = xmlGetProp(curNode, (const xmlChar *) "Pool-size");
        tpool->pool_size = atoi((char *) sValue) * 1024;
        xmlFree(sValue);
        //
        sValue = xmlGetProp(curNode, (const xmlChar *) "Reset-rate");
        tpool->reset_rate = atoi((char *) sValue);
        xmlFree(sValue);
    }
    //
    return ERR_SUCCESS;
}

int base_pool_parser(struct base_pool *bpool, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    if (NULL != curNode) {
        sValue = xmlGetProp(curNode, (const xmlChar *) "Avail");
        if (!strcasecmp("Enable", (char *) sValue)) bpool->avail = 0x01;
        else if (!strcasecmp("Disable", (char *) sValue)) bpool->avail = 0x00;
        xmlFree(sValue);
    }
    //
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(bpool->address.sin_addr, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Port");
            bpool->address.sin_port = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "DBName")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(bpool->database, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlGetProp(curNode, (const xmlChar *) "Name");
            strcpy(bpool->user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Password");
            strcpy(bpool->password, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "ReconTimeOut")) {
            sValue = xmlNodeGetContent(curNode);
            bpool->recon_timeout = atoi((char *) sValue) * 60; // s
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

int debug_level_value(char *level_txt) {
    int level_type;
    //
    if (!strcmp("FATAL", level_txt)) level_type = 0;
    else if (!strcmp("ERROR", level_txt)) level_type = 1;
    else if (!strcmp("WARN", level_txt)) level_type = 2;
    else if (!strcmp("INFO", level_txt)) level_type = 3;
    else if (!strcmp("DEBUG", level_txt)) level_type = 4;
    else if (!strcmp("TRACE", level_txt)) level_type = 5;
    else if (!strcmp("MAX", level_txt)) level_type = 6;
    else level_type = 6;
    //
    return level_type;
}

int log_config_parser(struct cont_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->runtime_log.logfile, CONTAIN_LOG_DEFAULT)
            } else strcpy(config->runtime_log.logfile, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "DebugLevel");
            config->runtime_log.debug_level = debug_level_value((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "AccessLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->access_log.logfile, CONTAIN_ACCESS_DEFAULT)
            } else strcpy(config->access_log.logfile, (char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

//
#define APPEND_PATH(APATH, SUBDIR) \
        strcpy(APATH, SUBDIR + 1)

static char *lscpy(char * dest, const char * src) {
    if (!dest) return NULL;
    register char *d = dest;
    register const char *s = src;
    while ((*d = *s++)) d++;
    return d;
}

int csp_lib_parser(vpa_lsth vlsth, char *csp_path, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    char lib_path[MAX_PATH];
    char *last_char;
    /* We don't care what the top level element name is */
    if (NULL != curNode) {
        sValue = xmlGetProp(curNode, (const xmlChar *) "LibName");
        last_char = lscpy(lib_path, csp_path);
        APPEND_PATH(last_char, (char *) sValue);
        xmlFree(sValue);
    }
    //
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Virtu-path")) {
            struct vpa_list *vlistmp = vlist_newvpa(vlsth);
            vlistmp->lib_path = strdup((char *) lib_path);
            //
            sValue = xmlNodeGetContent(curNode);
            vlistmp->vir_path = strdup((char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "CSP-name");
            vlistmp->csp_name = strdup((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

//

int path_map_parser(vpa_lsth vlsth, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    char csp_path[MAX_PATH];
    /* We don't care what the top level element name is */
    if (NULL != curNode) {
        sValue = xmlGetProp(curNode, (const xmlChar *) "CSP-path");
        strcpy(csp_path, (char *) sValue);
        xmlFree(sValue);
    }
    //
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "CSP-lib")) {
            if (csp_lib_parser(vlsth, csp_path, docXml, curNode))
                return ERR_FAULT;
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

int parse_serv_config(struct cont_config *config, const char *xml_config) {
    xmlDocPtr docXml = NULL;
    xmlNodePtr curNode = NULL;
    //
    docXml = xmlParseFile(xml_config);
    if (NULL == docXml) return ERR_FAULT;
    /** Check the document is of the right kind*/
    curNode = xmlDocGetRootElement(docXml);
    if (NULL == curNode) {
        //WriteErrorLog("empty document\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }
    if (xmlStrcmp(curNode->name, (const xmlChar *) "CSPContain")) {
        //WriteErrorLog("document of the wrong type, root node != desktop-config\n");
        xmlFreeDoc(docXml);
        return ERR_FAULT;
    }
    /** Now, walk the tree.*/
    /* First level is a list of service*/
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "DB-pool")) {
            if (base_pool_parser(&config->bpool, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "MEM-pool")) {
            if (memory_pool_parser(&config->tpool, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LogConfig")) {
            if (log_config_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Path-map")) {
            if (path_map_parser(&config->vlist, docXml, curNode))
                return ERR_FAULT;
        }
        curNode = curNode->next;
    }
    //
    xmlFreeDoc(docXml);
    /* Clean up everything else before quitting. */
    xmlCleanupParser();
    //
    return ERR_SUCCESS;
}

void serialize(char *sbuf, struct cont_config *config) {
    /* sprintf(sbuf, "serv_config is:\n"
                "\tmax_con:%d lock_file:[%s]\n"
                "\tpid_file:[%s]  tlog_file:[%s]\n"
                "\tauth_host:[%s]\tauth_port:%d\tuname:[%s]\n"
                "\tpasswd:[%s]\tdbname:[%s]\n",
                // Server info
                config->max_connecting,
                config->lock_file,
                config->pid_file,
                // config->sLogFile,
                // Auth DB
                config->cache_saddr);
     */
}

int serv_config_from_xml(struct cont_config *config, const char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct cont_config));
    if (parse_serv_config(config, xml_config)) return -1;
#ifdef _DEBUG
    //serialize(config);
#endif
    return 0;
}

struct cont_config *load_config_bzl(const char *config_path) {
    if (!config_path) return NULL;
    //
    strcpy(_config_path_, (char *) config_path);
    if (serv_config_from_xml(&_cont_config_, config_path)) return NULL;
    //
    return &_cont_config_;
}




