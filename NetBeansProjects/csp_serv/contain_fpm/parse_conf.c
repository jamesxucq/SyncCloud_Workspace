/* parse_conf.c
 * Created on: 2010-3-8
 * Author: Divad
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/parser.h>
#include <sys/un.h>

#include "error_code.h"
#include "file_utili.h"
#include "parse_conf.h"

//
#define DEFAULT_VALUE    "Default"

//
struct fpm_config _fpm_config_;
char _config_path_[MAX_PATH * 3];

//

int cont_fpm_parser(struct fpm_config *config, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "Fcgi-app")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_SBIN_PATH(config->fcgi_app, CFPM_CONTAIN_DEFAULT)
            } else strcpy(config->fcgi_app, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "User")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->user_name, CFPM_USER_DEFAULT);
            } else strcpy(config->user_name, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, BAD_CAST "Group");
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                strcpy(config->group_name, CFPM_GROUP_DEFAULT);
            } else strcpy(config->group_name, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Address")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->address.sin_addr, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "Port");
            config->address.sin_port = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "Childs")) {
            sValue = xmlNodeGetContent(curNode);
            config->fork_count = atoi((char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "UnixDomain")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->unix_domain, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "PidFile")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(config->pid_file, CSPFPM_PID_DEFAULT)
            } else strcpy(config->pid_file, (char *) sValue);
            xmlFree(sValue);
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "WorkingDirectory")) {
            sValue = xmlNodeGetContent(curNode);
            strcpy(config->work_directory, (char *) sValue);
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

int log_config_parser(log_config *runtime_log, xmlDocPtr docXml, xmlNodePtr curNode) {
    xmlChar *sValue = NULL;
    /* We don't care what the top level element name is */
    curNode = curNode->xmlChildrenNode;
    while (NULL != curNode) {
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "RuntimeLog")) {
            sValue = xmlNodeGetContent(curNode);
            if (!sValue || !strcasecmp(DEFAULT_VALUE, (char *) sValue)) {
                CREAT_LOGS_PATH(runtime_log->logfile, CSPFPM_LOG_DEFAULT)
            } else strcpy(runtime_log->logfile, (char *) sValue);
            xmlFree(sValue);
            //
            sValue = xmlGetProp(curNode, (const xmlChar *) "DebugLevel");
            runtime_log->debug_level = debug_level_value((char *) sValue);
            xmlFree(sValue);
        }
        curNode = curNode->next;
    }
    //
    return ERR_SUCCESS;
}

//

int parse_serv_config(struct fpm_config *config, const char *xml_config) {
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
        if (!xmlStrcmp(curNode->name, (const xmlChar *) "CSP-FPM")) {
            if (cont_fpm_parser(config, docXml, curNode))
                return ERR_FAULT;
        } else if (!xmlStrcmp(curNode->name, (const xmlChar *) "LogConfig")) {
            if (log_config_parser(&config->runtime_log, docXml, curNode))
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

void serialize(char *sbuf, struct fpm_config *config) {
    /* sprintf(sbuf, "fpm_config is:\n"
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

int serv_config_from_xml(struct fpm_config *config, const char *xml_config) {
    if (!config || !xml_config) return -1;
    memset(config, '\0', sizeof (struct fpm_config));
    if (parse_serv_config(config, xml_config)) return -1;
#ifdef _DEBUG
    //serialize(config);
#endif
    return 0;
}

struct fpm_config *load_config_bzl(const char *config_path) {
    if (!config_path) return NULL;
    //
    strcpy(_config_path_, (char *) config_path);
    if (serv_config_from_xml(&_fpm_config_, config_path)) return NULL;
    //
    return &_fpm_config_;
}

int check_config(struct fpm_config *config) {
    struct sockaddr_un un;
    //
    if (!config->fcgi_app[0]) {
        fprintf(stderr, "csp-fpm: no FastCGI application given\n");
        return -1;
    }
    if (0 == config->address.sin_port && !config->unix_domain[0]) {
        fprintf(stderr, "csp-fpm: no socket given (use either -p or -s)\n");
        return -1;
    } else if (0 != config->address.sin_port && config->unix_domain[0]) {
        fprintf(stderr, "csp-fpm: either a Unix domain socket or a TCP-port, but not both\n");
        return -1;
    }
    if (config->unix_domain[0] && strlen(config->unix_domain) > sizeof (un.sun_path) - 1) {
        fprintf(stderr, "csp-fpm: path of the Unix domain socket is too long\n");
        return -1;
    }
    /* SUID handling */
    if (!getuid() && (geteuid() != getuid() || getegid() != getgid())) {
        fprintf(stderr, "csp-fpm: Are you nuts? Don't apply a SUID bit to this binary\n");
        return -1;
    }
    //
    return 0x00;
}





