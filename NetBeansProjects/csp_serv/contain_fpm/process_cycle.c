#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <wait.h>
#include <grp.h>
#include <pwd.h>

#include "error_code.h"
#include "logger.h"
#include "file_utili.h"
#include "process_cycle.h"

//
#define FCGI_LISTENSOCK_FILENO 0

//

static void worker_process_init(struct process_cycle *proce_cycle) {
    /*
        if (ngx_set_environment(cycle, NULL) == NULL) {
            exit(2);
        }
        if (worker >= 0 && ccf->priority != 0) {
            if (setpriority(PRIO_PROCESS, 0, ccf->priority) == -1) {
                _LOG_ERROR("setpriority(%d) failed", ccf->priority);
            }
        }
        if (ccf->rlimit_nofile != CONF_UNSET) {
            rlmt.rlim_cur = (rlim_t) ccf->rlimit_nofile;
            rlmt.rlim_max = (rlim_t) ccf->rlimit_nofile;
            if (setrlimit(RLIMIT_NOFILE, &rlmt) == -1) {
                _LOG_ERROR("setrlimit(RLIMIT_NOFILE, %i) failed", ccf->rlimit_nofile);
            }
        }
        if (ccf->rlimit_core != CONF_UNSET) {
            rlmt.rlim_cur = (rlim_t) ccf->rlimit_core;
            rlmt.rlim_max = (rlim_t) ccf->rlimit_core;
            if (setrlimit(RLIMIT_CORE, &rlmt) == -1) {
                _LOG_ERROR("setrlimit(RLIMIT_CORE, %O) failed", ccf->rlimit_core);
            }
        }
     */
    //
    if (0x00 == geteuid()) {
        if (0 == proce_cycle->group_id) {
            _LOG_WARN("csp-fpm: I will not set gid to 0");
            return;
        }
        //
        if (setgid(proce_cycle->group_id) == -1) {
            _LOG_FATAL("csp-fpm: setgid(%d) failed", proce_cycle->group_id);
            exit(2); /* fatal */
        }
        //
        if (initgroups(proce_cycle->user_name, proce_cycle->group_id) == -1) {
            _LOG_FATAL("csp-fpm: initgroups(%s, %d) failed", proce_cycle->user_name, proce_cycle->group_id);
        }
        if (setuid(proce_cycle->user_id) == -1) {
            _LOG_FATAL("csp-fpm: setuid(%d) failed", proce_cycle->user_id);
            exit(2); /* fatal */
        }
    }
}

//

pid_t start_worker_process(struct process_cycle *proce_cycle) {
    pid_t child = vfork();
    switch (child) {
        case -1:
            _LOG_WARN("csp-fpm: fork failed: %s\n", strerror(errno));
        case 0: // child
            worker_process_init(proce_cycle);
        {
            if (proce_cycle->fcgi_fd != FCGI_LISTENSOCK_FILENO) {
                close(FCGI_LISTENSOCK_FILENO);
                dup2(proce_cycle->fcgi_fd, FCGI_LISTENSOCK_FILENO);
                close(proce_cycle->fcgi_fd);
            }
            /* fork and replace shell */
            execv(proce_cycle->fcgi_app, NULL);
            /* in nofork mode stderr is still open */
            _LOG_WARN("csp-fpm: exec failed: %s %s\n", proce_cycle->fcgi_app, strerror(errno));
            exit(errno);
            break;
        }
            break;
        default: // parent
            break;
    }
    //
    return child;
}

//

int master_process_cycle(struct process_cycle *proce_cycle) {
    struct timeval tv = {0, 100 * 1000};
    pid_t child;
    unsigned int inde;
    //
    for (inde = 0x00; proce_cycle->fork_count > inde; inde++) {
        _LOG_WARN("csp-fpm: start worker process! inde:%d", inde);
        child = start_worker_process(proce_cycle);
        select(0, NULL, NULL, NULL, &tv);
        _LOG_WARN("csp-fpm: child spawned successfully: PID: %d", child);
        if (-1 != proce_cycle->pid_fd) { /* write pid file */ /* assume a 32bit pid_t */
            char pidbuf[12];
            snprintf(pidbuf, sizeof (pidbuf) - 1, "%d\n", child);
            if (-1 == write_all(proce_cycle->pid_fd, pidbuf, strlen(pidbuf))) {
                _LOG_WARN("csp-fpm: writing pid file failed: %s", strerror(errno));
                close(proce_cycle->pid_fd);
                proce_cycle->pid_fd = -1;
            }
        }
    }
    //
    int status;
    for (;;) {
        switch (wait(&status)) {
            case -1:
                _LOG_ERROR("csp-fpm: master process wait error! exited.");
                return ERR_FAULT;
                break;
            default:
                if (WIFEXITED(status)) {
                    _LOG_WARN("csp-fpm: child exited with: %d", WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    _LOG_WARN("csp-fpm: child signaled: %d\n", WTERMSIG(status));
                } else {
                    _LOG_WARN("csp-fpm: child died somehow: exit status = %d", status);
                }
                break;
        }
        //
        _LOG_WARN("csp-fpm: restart worker process!");
        child = start_worker_process(proce_cycle);
        select(0, NULL, NULL, NULL, &tv);
        _LOG_WARN("csp-fpm: child spawned successfully: PID: %d", child);
        if (-1 != proce_cycle->pid_fd) { /* write pid file */ /* assume a 32bit pid_t */
            char pidbuf[12];
            snprintf(pidbuf, sizeof (pidbuf) - 1, "%d\n", child);
            if (-1 == write_all(proce_cycle->pid_fd, pidbuf, strlen(pidbuf))) {
                _LOG_WARN("csp-fpm: writing pid file failed: %s", strerror(errno));
                close(proce_cycle->pid_fd);
                proce_cycle->pid_fd = -1;
            }
        }
    }
    //
    return ERR_SUCCESS;
}



