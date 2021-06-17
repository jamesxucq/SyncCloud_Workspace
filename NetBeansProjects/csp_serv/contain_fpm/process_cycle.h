/* 
 * File:   process_cycle.h
 * Author: Administrator
 *
 * Created on 2016.12.1, PM 3:19
 */

#ifndef PROCESS_CYCLE_H
#define PROCESS_CYCLE_H

#ifdef __cplusplus
extern "C" {
#endif

    struct process_cycle {
        char *fcgi_app;
        int fcgi_fd;
        int fork_count;
        int pid_fd;
        char *user_name;
        uid_t user_id;
        gid_t group_id;
    };

    int master_process_cycle(struct process_cycle *proce_cycle);


#ifdef __cplusplus
}
#endif

#endif /* PROCESS_CYCLE_H */

