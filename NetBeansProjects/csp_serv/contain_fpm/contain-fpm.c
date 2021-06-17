
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

//
#include "error_code.h"
#include "options.h"
#include "file_utili.h"
#include "parse_conf.h"
#include "daemonize.h"
#include  "fpm_socket.h"
#include "logger.h"
#include "process_cycle.h"

//

static void print_debug(char *xml_config, struct fpm_config *config, struct process_cycle *proce_cycle) {
    _LOG_DEBUG("xml_config:%s", xml_config);
    _LOG_DEBUG("fcgi_app:%s user_name:%s group_name:%s unix_domain:%s "
            "fork_count:%d pid_file:%s work_directory:%s address.sin_addr:%s address.sin_port:%d",
            config->fcgi_app, config->user_name, config->group_name, config->unix_domain,
            config->fork_count, config->pid_file, config->work_directory, config->address.sin_addr, config->address.sin_port);
    _LOG_DEBUG("fcgi_app:%s fcgi_fd:%d fork_count:%d pid_fd:%d user_name:%s user_id:%d group_id:%d ",
            proce_cycle->fcgi_app, proce_cycle->fcgi_fd, proce_cycle->fork_count, proce_cycle->pid_fd,
            proce_cycle->user_name, proce_cycle->user_id, proce_cycle->group_id);
}

int main(int argc, char *argv[]) {
    char xml_config[MAX_PATH * 3];
    if (parse_args(xml_config, argc, argv)) return -1;
    if (FILE_EXIST(xml_config)) {
        fprintf(stderr, "config file not exist. %s\n", xml_config);
        return -1;
    }
    //
    struct fpm_config *config = load_config_bzl(xml_config);
    if (check_config(config)) {
        fprintf(stderr, "config file error.\n");
        return -1;
    }

#ifndef _DEBUG
    if (daemonize(config->work_directory)) return ERR_FAULT;
#endif
    log_inital(config->runtime_log.logfile, config->runtime_log.debug_level);
    //
    int pid_fd = open_pid_file(config->pid_file);
    if (-1 == pid_fd) {
        _LOG_WARN("csp-fpm: opening PID-file failed");
        return -1;
    }
    //
    int fcgi_fd = -1;
    mode_t sockmode = (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) & ~read_umask();
    uid_t user_id = 0;
    gid_t group_id = 0;
    int backlog = 1024;
    char *unix_socket = NULL;
    if (config->unix_domain[0]) unix_socket = config->unix_domain;
    if (!getuid()) {
        if (-1 == find_user_group(config->user_name, config->group_name, &user_id, &group_id))
            return -1;
    }
    if (-1 == (fcgi_fd = bind_socket(config->address.sin_addr, config->address.sin_port, unix_socket, user_id, group_id, sockmode, backlog)))
        return -1;
    //
    struct process_cycle proce_cycle;
    proce_cycle.fcgi_app = config->fcgi_app;
    proce_cycle.fcgi_fd = fcgi_fd;
    proce_cycle.fork_count = config->fork_count;
    proce_cycle.pid_fd = pid_fd;
    proce_cycle.user_name = config->user_name;
    proce_cycle.user_id = user_id;
    proce_cycle.group_id = group_id;
#ifdef _DEBUG
    print_debug(xml_config, config, &proce_cycle);
#endif
    master_process_cycle(&proce_cycle);
    //
    if (-1 != pid_fd) close(pid_fd);
    close(fcgi_fd);
    log_final();
    return ERR_SUCCESS;
}


