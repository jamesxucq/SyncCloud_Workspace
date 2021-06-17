/* 
 * File:   fpm_socket.h
 * Author: Administrator
 *
 * Created on 2016.12.1, PM 3:30
 */

#ifndef FPM_SOCKET_H
#define FPM_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(HAVE_IPV6) && defined(HAVE_INET_PTON)
#define USE_IPV6
#endif

    mode_t read_umask(void);
    int bind_socket(const char *addr, unsigned short port, const char *unixsocket, uid_t uid, gid_t gid, mode_t mode, int backlog);
    int find_user_group(const char *user, const char *group, uid_t *uid, gid_t *gid);


#ifdef __cplusplus
}
#endif

#endif /* FPM_SOCKET_H */

