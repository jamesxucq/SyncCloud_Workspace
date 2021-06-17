// #include <stdio.h>
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qdecoder.h>
#include <mysql/mysql.h>

#include <qstri.h>
#include <tiny_pool.h>

//
#define LINE_TXT 1024
#define PAGE_BUFFER_SIZE 0x2000  // 8K
#define BUFFER_AUTO_FLUSH 1
#define SET_CONTENT_TYPE 1

//
#define mkzero(TEXT) TEXT[0] = '\0'

// #define DEFAULT_CONTENT_TYPE "Content-type: text/plain\n\n"
#define DEFAULT_CONTENT_TYPE "Content-type: text/html\n\n"

//
#define DEFAULT_QPARSE_METHOD Q_CGI_GET

// qcgireq.c
#define qcgireq_setoption_bzl(filemode, basepath, clearold) \
    qcgireq_setoption(_req_, filemode, basepath, clearold, &recon)
#define qcgireq_parse_bzl(method) \
    qcgireq_parse(_req_, method, &recon)
#define qcgireq_getquery_bzl(method) qcgireq_getquery(method)

// qcgires.c
#define qcgires_setcookie_bzl(name, value, expire, path, domain, secure) \
    qcgires_setcookie(_req_, name, value, expire, path, domain, secure)
#define qcgires_removecookie_bzl(name, path, domain, secure) \
    qcgires_removecookie(_req_, name, path, domain, secure)
#define qcgires_setcontenttype_bzl(mimetype) \
    qcgires_setcontenttype(_req_, mimetype)
#define qcgires_getcontenttype_bzl() \
    qcgires_getcontenttype(_req_)
#define qcgires_redirect_bzl(uri) \
    qcgires_redirect(_req_, uri)
#define qcgires_download_bzl(filepath, mimetype) \
    qcgires_download(_req_, filepath, mimetype)
#define qcgires_error_bzl(format, ...) { \
    qcgires_error(request, format, ##__VA_ARGS__); \
    return NULL; \
}

// qcgisess.c
// #define qcgisess_init_bzl(dirpath) qcgisess_init(_req_, dirpath) // disable for spc default
#define qcgisess_settimeout_bzl(seconds) qcgisess_settimeout(_sess_, seconds)
#define qcgisess_getid_bzl() qcgisess_getid(_sess_)
#define qcgisess_getcreated_bzl() qcgisess_getcreated(_sess_)
#define qcgisess_save_bzl() qcgisess_save(_sess_)
#define qcgisess_destroy_bzl() qcgisess_destroy(_sess_)