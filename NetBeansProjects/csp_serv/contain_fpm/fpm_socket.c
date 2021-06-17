/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <grp.h>
#include <pwd.h>

#include "logger.h"
#include "fpm_socket.h"
//

mode_t read_umask(void) {
    mode_t mask = umask(0);
    umask(mask);
    return mask;
}

int bind_socket(const char *addr, unsigned short port, const char *unix_socket, uid_t uid, gid_t gid, mode_t mode, int backlog) {
    int fcgi_fd, socket_type, val;
    //
    struct sockaddr_un fcgi_addr_un;
    struct sockaddr_in fcgi_addr_in;
#ifdef USE_IPV6
    struct sockaddr_in6 fcgi_addr_in6;
#endif
    struct sockaddr *fcgi_addr;
    socklen_t servlen;

    if (unix_socket) {
        memset(&fcgi_addr_un, 0, sizeof (fcgi_addr_un));
        fcgi_addr_un.sun_family = AF_UNIX;
        /* already checked in main() */
        if (strlen(unix_socket) > sizeof (fcgi_addr_un.sun_path) - 1) return -1;
        strcpy(fcgi_addr_un.sun_path, unix_socket);
#ifdef SUN_LEN
        servlen = SUN_LEN(&fcgi_addr_un);
#else
        /* stevens says: */
        servlen = strlen(fcgi_addr_un.sun_path) + sizeof (fcgi_addr_un.sun_family);
#endif
        socket_type = AF_UNIX;
        fcgi_addr = (struct sockaddr *) &fcgi_addr_un;
        /* check if some backend is listening on the socket
         * as if we delete the socket-file and rebind there will be no "socket already in use" error
         */
        if (-1 == (fcgi_fd = socket(socket_type, SOCK_STREAM, 0))) {
            fprintf(stderr, "csp-fpm: couldn't create socket: %s\n", strerror(errno));
            return -1;
        }
        if (0 == connect(fcgi_fd, fcgi_addr, servlen)) {
            fprintf(stderr, "csp-fpm: socket is already in use, can't spawn\n");
            close(fcgi_fd);
            return -1;
        }
        /* cleanup previous socket if it exists */
        if (-1 == unlink(unix_socket)) {
            switch (errno) {
                case ENOENT:
                    break;
                default:
                    fprintf(stderr, "csp-fpm: removing old socket failed: %s\n", strerror(errno));
                    close(fcgi_fd);
                    return -1;
            }
        }
        close(fcgi_fd);
    } else {
        memset(&fcgi_addr_in, 0, sizeof (fcgi_addr_in));
        fcgi_addr_in.sin_family = AF_INET;
        fcgi_addr_in.sin_port = htons(port);

        servlen = sizeof (fcgi_addr_in);
        socket_type = AF_INET;
        fcgi_addr = (struct sockaddr *) &fcgi_addr_in;
#ifdef USE_IPV6
        memset(&fcgi_addr_in6, 0, sizeof (fcgi_addr_in6));
        fcgi_addr_in6.sin6_family = AF_INET6;
        fcgi_addr_in6.sin6_port = fcgi_addr_in.sin_port;
#endif
        if (addr == NULL) {
            fcgi_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
#ifdef HAVE_INET_PTON
        } else if (1 == inet_pton(AF_INET, addr, &fcgi_addr_in.sin_addr)) {
            /* nothing to do */
#ifdef HAVE_IPV6
        } else if (1 == inet_pton(AF_INET6, addr, &fcgi_addr_in6.sin6_addr)) {
            servlen = sizeof (fcgi_addr_in6);
            socket_type = AF_INET6;
            fcgi_addr = (struct sockaddr *) &fcgi_addr_in6;
#endif
        } else {
            fprintf(stderr, "csp-fpm: '%s' is not a valid IP address\n", addr);
            return -1;
#else
        } else {
            if ((in_addr_t) (-1) == (fcgi_addr_in.sin_addr.s_addr = inet_addr(addr))) {
                fprintf(stderr, "csp-fpm: '%s' is not a valid IPv4 address\n", addr);
                return -1;
            }
#endif
        }
    }

    if (-1 == (fcgi_fd = socket(socket_type, SOCK_STREAM, 0))) {
        fprintf(stderr, "csp-fpm: couldn't create socket: %s\n", strerror(errno));
        return -1;
    }
    val = 1;
    if (setsockopt(fcgi_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof (val)) < 0) {
        fprintf(stderr, "csp-fpm: couldn't set SO_REUSEADDR: %s\n", strerror(errno));
        close(fcgi_fd);
        return -1;
    }
    if (-1 == bind(fcgi_fd, fcgi_addr, servlen)) {
        fprintf(stderr, "csp-fpm: bind failed: %s\n", strerror(errno));
        close(fcgi_fd);
        return -1;
    }
    if (unix_socket) {
        if (-1 == chmod(unix_socket, mode)) {
            fprintf(stderr, "csp-fpm: couldn't chmod socket: %s\n", strerror(errno));
            close(fcgi_fd);
            unlink(unix_socket);
            return -1;
        }
        if (0 != uid || 0 != gid) {
            if (0 == uid) uid = -1;
            if (0 == gid) gid = -1;
            if (-1 == chown(unix_socket, uid, gid)) {
                fprintf(stderr, "csp-fpm: couldn't chown socket: %s\n", strerror(errno));
                close(fcgi_fd);
                unlink(unix_socket);
                return -1;
            }
        }
    }
    if (-1 == listen(fcgi_fd, backlog)) {
        fprintf(stderr, "csp-fpm: listen failed: %s\n", strerror(errno));
        close(fcgi_fd);
        if (unix_socket) unlink(unix_socket);
        return -1;
    }
    return fcgi_fd;
}

/*
    if (0x00 == geteuid()) {
        struct passwd *pwd = getpwnam(config->user);
        if (pwd == NULL) {
            _LOG_ERROR("getpwnam(\"%s\") failed", config->user);
        }
        //
        if (setgid(pwd->pw_gid) == -1) {
            _LOG_FATAL("setgid(%d) failed", pwd->pw_gid);
            exit(2); // fatal
        }
        if (initgroups(config->user, pwd->pw_gid) == -1) {
            _LOG_FATAL("initgroups(%s, %d) failed", config->user, pwd->pw_gid);
        }
        if (setuid(pwd->pw_uid) == -1) {
            _LOG_FATAL("setuid(%d) failed", pwd->pw_uid);
            exit(2); // fatal
        }
    }
 */

int find_user_group(const char *user, const char *group, uid_t *uid, gid_t *gid) {
    struct passwd *my_pwd = getpwnam(user);
    if (NULL == my_pwd) {
        _LOG_ERROR("csp-fpm: getpwnam(\"%s\") failed", user);
        return -1;
    }
    struct group *my_grp = getgrnam(group);
    if (NULL == my_grp) {
        _LOG_ERROR("csp-fpm: can't find group name %s\n", group);
        return -1;
    }
    if (0 == my_grp->gr_gid) {
        _LOG_WARN("csp-fpm: I will not set gid to 0\n");
        return 0;
    }
    //
    *uid = my_pwd->pw_uid;
    *gid = my_grp->gr_gid;
    return 0;
}