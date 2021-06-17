
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <dav_cloud.h> // add by james 20161115

#define NGX_HTTP_DAV_OFF             2

#define NGX_HTTP_DAV_NO_DEPTH        -3
#define NGX_HTTP_DAV_INVALID_DEPTH   -2
#define NGX_HTTP_DAV_INFINITY_DEPTH  -1

//
typedef struct {
    ngx_uint_t methods;
    ngx_uint_t access;
    ngx_uint_t min_delete_depth;
    ngx_flag_t create_full_put_path;
} ngx_http_dav_loc_conf_t;

typedef struct {
    ngx_str_t path;
    size_t len;
} ngx_http_dav_mvcp_ctx_t;

//
typedef struct tree_ctx_s  tree_ctx_t;
typedef ngx_int_t (*tree_tree_handler_pt) (tree_ctx_t *ctx, ngx_str_t *name);
typedef ngx_int_t (*tree_file_handler_pt) (tree_ctx_t *ctx, ngx_str_t *name, size_t root_len);

struct tree_ctx_s {
    off_t                      size;
    ngx_uint_t                 access;
    time_t                     mtime;
    tree_file_handler_pt       file_handler;
    tree_tree_handler_pt       pre_tree_handler;
    tree_tree_handler_pt        post_tree_handler;
    tree_file_handler_pt       spec_handler;
    void                       *data;
    ngx_log_t                  *log;
};

// add by james 20161115
#define FILE_SIZE(SIZE, PATH, LOG) { \
    struct stat64 statb; \
    if (stat64((const char*)PATH, &statb) < 0) \
        ngx_log_error(NGX_LOG_ERR, LOG, NGX_EISDIR, "DAV: fstat errno:%d", errno); \
    SIZE = statb.st_size; \
}

static int link_kalive;
#define LINK_KALIVE \
    if (!POW2N_MOD(++link_kalive, 512)) cache_kalive_ine();
static int data_kalive;
#define DATA_KALIVE \
    if (!POW2N_MOD(++data_kalive, 8)) cache_kalive_ine();

//
static ngx_int_t ngx_http_dav_handler(ngx_http_request_t *r);
static void ngx_http_dav_put_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_dav_delete_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_dav_mkcol_handler(ngx_http_request_t *r, ngx_http_dav_loc_conf_t *dlcf);
static ngx_int_t ngx_http_dav_move_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_dav_copy_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_dav_location(ngx_http_request_t *r, u_char *path);
static void *ngx_http_dav_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_dav_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t dav_cloud_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_dav_init(ngx_conf_t *cf);
static ngx_int_t dav_walk_tree(tree_ctx_t *ctx, ngx_str_t *tree, size_t root_len);

//
static ngx_conf_bitmask_t ngx_http_dav_methods_mask[] = {
    { ngx_string("off"), NGX_HTTP_DAV_OFF},
    { ngx_string("put"), NGX_HTTP_PUT},
    { ngx_string("delete"), NGX_HTTP_DELETE},
    { ngx_string("mkcol"), NGX_HTTP_MKCOL},
    { ngx_string("copy"), NGX_HTTP_COPY},
    { ngx_string("move"), NGX_HTTP_MOVE},
    { ngx_null_string, 0}
};

static ngx_command_t ngx_http_dav_commands[] = {
    { ngx_string("dav_methods"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_1MORE,
        ngx_conf_set_bitmask_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dav_loc_conf_t, methods),
        &ngx_http_dav_methods_mask},
    { ngx_string("create_full_put_path"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
        ngx_conf_set_flag_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dav_loc_conf_t, create_full_put_path),
        NULL},
    { ngx_string("min_delete_depth"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        ngx_conf_set_num_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dav_loc_conf_t, min_delete_depth),
        NULL},
    { ngx_string("dav_access"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE123,
        ngx_conf_set_access_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_dav_loc_conf_t, access),
        NULL},
    ngx_null_command
};

static ngx_http_module_t ngx_http_dav_module_ctx = {
    dav_cloud_init, /* preconfiguration */
    ngx_http_dav_init, /* postconfiguration */
    NULL, /* create main configuration */
    NULL, /* init main configuration */
    NULL, /* create server configuration */
    NULL, /* merge server configuration */
    ngx_http_dav_create_loc_conf, /* create location configuration */
    ngx_http_dav_merge_loc_conf /* merge location configuration */
};

ngx_module_t ngx_http_dav_module = {
    NGX_MODULE_V1,
    &ngx_http_dav_module_ctx, /* module context */
    ngx_http_dav_commands, /* module directives */
    NGX_HTTP_MODULE, /* module type */
    NULL, /* init master */
    NULL, /* init module */
    NULL, /* init process */
    NULL, /* init thread */
    NULL, /* exit thread */
    NULL, /* exit process */
    NULL, /* exit master */
    NGX_MODULE_V1_PADDING
};

//

static ngx_int_t ngx_http_dav_handler(ngx_http_request_t *r) {
    ngx_int_t rc;
    ngx_http_dav_loc_conf_t *dlcf;

    dlcf = ngx_http_get_module_loc_conf(r, ngx_http_dav_module);
    if (!(r->method & dlcf->methods)) {
        return NGX_DECLINED;
    }
    switch (r->method) {
        case NGX_HTTP_PUT:
            if (r->uri.data[r->uri.len - 1] == '/') {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: cannot PUT to a collection");
                return NGX_HTTP_CONFLICT;
            }
            r->request_body_in_file_only = 1;
            r->request_body_in_persistent_file = 1;
            r->request_body_in_clean_file = 1;
            r->request_body_file_group_access = 1;
            r->request_body_file_log_level = 0;
            rc = ngx_http_read_client_request_body(r, ngx_http_dav_put_handler);
            if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
                return rc;
            }
            return NGX_DONE;
        case NGX_HTTP_DELETE:
            return ngx_http_dav_delete_handler(r);
        case NGX_HTTP_MKCOL:
            return ngx_http_dav_mkcol_handler(r, dlcf);
        case NGX_HTTP_COPY:
            return ngx_http_dav_copy_handler(r);
        case NGX_HTTP_MOVE:
            return ngx_http_dav_move_handler(r);
    }

    return NGX_DECLINED;
}

//
static ngx_int_t dav_path_depth(ngx_http_request_t *r, ngx_int_t dflt) {
    ngx_table_elt_t *depth;
//
    depth = r->headers_in.depth;
    if (depth == NULL) {
        return dflt;
    }
    if (depth->value.len == 1) {
        if (depth->value.data[0] == '0') {
            return 0;
        }
        if (depth->value.data[0] == '1') {
            return 1;
        }
    } else {
        if (depth->value.len == sizeof ("infinity") - 1 && ngx_strcmp(depth->value.data, "infinity") == 0) {
            return NGX_HTTP_DAV_INFINITY_DEPTH;
        }
    }
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent invalid \"Depth\" header: \"%V\"", &depth->value);
//
    return NGX_HTTP_DAV_INVALID_DEPTH;
}

static ngx_int_t dav_http_errno(ngx_log_t *log, ngx_err_t err, ngx_int_t not_found, char *failed, u_char *path) {
    ngx_int_t rc;
    ngx_uint_t level;
//
    if (err == NGX_ENOENT || err == NGX_ENOTDIR || err == NGX_ENAMETOOLONG) {
        level = NGX_LOG_ERR;
        rc = not_found;
    } else if (err == NGX_EACCES || err == NGX_EPERM) {
        level = NGX_LOG_ERR;
        rc = NGX_HTTP_FORBIDDEN;
    } else if (err == NGX_EEXIST) {
        level = NGX_LOG_ERR;
        rc = NGX_HTTP_NOT_ALLOWED;
    } else if (err == NGX_ENOSPC) {
        level = NGX_LOG_CRIT;
        rc = NGX_HTTP_INSUFFICIENT_STORAGE;
    } else {
        level = NGX_LOG_CRIT;
        rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    ngx_log_error(level, log, err, "DAV: %s \"%s\" failed", failed, path);
//
    return rc;
}

//
static ngx_int_t dav_tree_noop(tree_ctx_t *ctx, ngx_str_t *path) {
    return NGX_OK;
}
static ngx_int_t dav_file_noop(tree_ctx_t *ctx, ngx_str_t *path, size_t root_len) {
    return NGX_OK;
}

//
static ngx_int_t dav_delete_dir(tree_ctx_t *ctx, ngx_str_t *path) {
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http delete dir: \"%s\"", path->data);
    if (ngx_delete_dir(path->data) == NGX_FILE_ERROR) {
        /* TODO: add to 207 */
        (void) dav_http_errno(ctx->log, ngx_errno, 0, ngx_delete_dir_n, path->data);
    }
    return NGX_OK;
}

static ngx_int_t dav_delete_file(tree_ctx_t *ctx, ngx_str_t *path, size_t root_len) {
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http delete file: \"%s\"", path->data);
    if (cloud_delete(path->data + root_len, ctx->size)) {
        /* TODO: add to 207 */
        (void) dav_http_errno(ctx->log, ngx_errno, 0, ngx_delete_file_n, path->data);
    }
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "Cloud: dele_insert_ine: |%s|", path->data + root_len);
    dele_insert_ine(path->data + root_len, ctx->size)
    LINK_KALIVE
    //
    return NGX_OK;
}

static ngx_int_t dav_delete_spec(tree_ctx_t *ctx, ngx_str_t *path, size_t root_len) {
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http delete file: \"%s\"", path->data);
    if (ngx_delete_file(path->data) == NGX_FILE_ERROR) {
        /* TODO: add to 207 */
        (void) dav_http_errno(ctx->log, ngx_errno, 0, ngx_delete_file_n, path->data);
    }
    return NGX_OK;
}

static ngx_int_t dav_delete_path(ngx_log_t *log, ngx_str_t *path, size_t root_len, ngx_uint_t is_dir) {
    char *failed;
    tree_ctx_t tree;
//
    if (is_dir) {
        tree.file_handler = dav_delete_file;
        tree.pre_tree_handler = dav_tree_noop;
        tree.post_tree_handler = dav_delete_dir;
        tree.spec_handler = dav_delete_spec;
        tree.data = NULL;
        tree.log = log;
        /* TODO: 207 */
        if (dav_walk_tree(&tree, path, root_len) != NGX_OK) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
        if (ngx_delete_dir(path->data) != NGX_FILE_ERROR) {
            return NGX_OK;
        }
        failed = ngx_delete_dir_n;
    } else {
        uint64 file_size;
        FILE_SIZE(file_size, path->data, log)
        if (!cloud_delete(path->data + root_len, file_size)) {
            ngx_log_error(NGX_LOG_ERR, log, 0, "Cloud: dele_insert_ine: |%s|", path->data + root_len);
            dele_insert_ine(path->data + root_len, file_size)
            return NGX_OK;
        }
        failed = ngx_delete_file_n;
    }
//
    return dav_http_errno(log, ngx_errno, NGX_HTTP_NOT_FOUND, failed, path->data);
}

static void ngx_http_dav_put_handler(ngx_http_request_t *r) {
    size_t root_len;
    ngx_int_t rc;
    time_t date;
    ngx_str_t *temp, path;
    ngx_uint_t status;
    ngx_file_info_t fi;
    ngx_ext_rename_file_t ext;
    ngx_http_dav_loc_conf_t *dlcf;

    if (r->request_body == NULL || r->request_body->temp_file == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }
    if (ngx_http_map_uri_to_path(r, &path, &root_len, 0) == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }
    path.len--;
    temp = &r->request_body->temp_file->file.name;
    // add by james 20161115
    root_len = path.len - r->unparsed_uri.len;
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http put filename: \"%s\"", path.data);
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: Put: headers_in.user.data: %s", r->headers_in.user.data);
    if ((rc = increm_locked_ine(&r->headers_in.user))) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: \"%s\" lock user space error!", path.data);
        if (ngx_delete_file(temp->data) == NGX_FILE_ERROR) {
            ngx_log_error(NGX_LOG_CRIT, r->connection->log, ngx_errno, ngx_delete_file_n "DAV: \"%s\" failed", temp->data);
        }
        if(0x01 == rc) {
            ngx_http_finalize_request(r, NGX_HTTP_INSUFFICIENT_STORAGE);
            return;
        }
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }
    //
    if (ngx_file_info(path.data, &fi) == NGX_FILE_ERROR) {  /* destination does not exist */
        status = NGX_HTTP_CREATED;
    } else { /* destination exists */
        status = NGX_HTTP_NO_CONTENT;
        if (ngx_is_dir(&fi)) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: \"%s\" could not be created", path.data);
            if (ngx_delete_file(temp->data) == NGX_FILE_ERROR) {
                ngx_log_error(NGX_LOG_CRIT, r->connection->log, ngx_errno, ngx_delete_file_n "DAV: \"%s\" failed", temp->data);
            }
            unlock_space_ine(&r->headers_in.user);
            ngx_http_finalize_request(r, NGX_HTTP_CONFLICT);
            return;
        } else {
            dav_delete_path(r->connection->log, &path, root_len, 0x00);
        }
    }
    dlcf = ngx_http_get_module_loc_conf(r, ngx_http_dav_module);
    ext.access = dlcf->access;
    ext.path_access = dlcf->access;
    ext.time = -1;
    ext.create_path = dlcf->create_full_put_path;
    ext.delete_file = 1;
    ext.log = r->connection->log;
    if (r->headers_in.date) {
        date = ngx_parse_http_time(r->headers_in.date->value.data, r->headers_in.date->value.len);
        if (date != NGX_ERROR) {
            ext.time = date;
            ext.fd = r->request_body->temp_file->file.fd;
        }
    }
    if (ngx_ext_rename_file(temp, &path, &ext) != NGX_OK) {
        // add by james 20161115
        unlock_space_ine(&r->headers_in.user);
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }
    if (status == NGX_HTTP_CREATED) {
        if (ngx_http_dav_location(r, path.data) != NGX_OK) {
            unlock_space_ine(&r->headers_in.user);
            ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }
        r->headers_out.content_length_n = 0;
    }
    r->headers_out.status = status;
    r->header_only = 1;
    ngx_http_finalize_request(r, ngx_http_send_header(r));
    // add by james 20161115
    uint64 file_size;
    FILE_SIZE(file_size, path.data, r->connection->log)
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Cloud: addi_update_ine: |%s|", path.data + root_len);
    addi_update_ine(&r->headers_in.user, path.data + root_len, file_size)
}

//
static ngx_int_t ngx_http_dav_delete_handler(ngx_http_request_t *r) {
    size_t root_len;
    ngx_int_t rc, depth;
    ngx_uint_t i, d, is_dir;
    ngx_str_t path;
    ngx_file_info_t fi;
    ngx_http_dav_loc_conf_t *dlcf;

    if (r->headers_in.content_length_n > 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: DELETE with body is unsupported");
        return NGX_HTTP_UNSUPPORTED_MEDIA_TYPE;
    }

    dlcf = ngx_http_get_module_loc_conf(r, ngx_http_dav_module);
    if (dlcf->min_delete_depth) {
        d = 0;
        for (i = 0; i < r->uri.len; /* void */) {
            if (r->uri.data[i++] == '/') {
                if (++d >= dlcf->min_delete_depth && i < r->uri.len) {
                    goto ok;
                }
            }
        }
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: insufficient URI depth:%i to DELETE", d);
        return NGX_HTTP_CONFLICT;
    }
ok:
    if (ngx_http_map_uri_to_path(r, &path, &root_len, 0) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    path.len--;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http delete filename: \"%s\"", path.data);
    if (ngx_link_info(path.data, &fi) == NGX_FILE_ERROR) {
        rc = (errno == NGX_ENOTDIR) ? NGX_HTTP_CONFLICT : NGX_HTTP_NOT_FOUND;
        return dav_http_errno(r->connection->log, errno, rc, ngx_link_info_n, path.data);
    }

    if (ngx_is_dir(&fi)) {
        /* if (r->uri.data[r->uri.len - 1] != '/') { // disable by james 20161120 for client bug
            ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: DELETE \"%s\" failed", path.data);
            return NGX_HTTP_CONFLICT;
        } */
        depth = dav_path_depth(r, NGX_HTTP_DAV_INFINITY_DEPTH);
        if (depth != NGX_HTTP_DAV_INFINITY_DEPTH) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be infinity");
            return NGX_HTTP_BAD_REQUEST;
        }
        // path.len--; /* omit "/\0" */
        is_dir = 1;
    } else {
        /* we do not need to test (r->uri.data[r->uri.len - 1] == '/')
         * because ngx_link_info("/file/") returned NGX_ENOTDIR above */
        depth = dav_path_depth(r, 0);
        if (depth != 0 && depth != NGX_HTTP_DAV_INFINITY_DEPTH) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be 0 or infinity");
            return NGX_HTTP_BAD_REQUEST;
        }
        is_dir = 0;
    }
    // add by james 20161115
    root_len = path.len - r->unparsed_uri.len;
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: delete headers_in.user.data: %s", r->headers_in.user.data);
    if (locked_space_ine(&r->headers_in.user)) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: \"%s\" lock user space error!", path.data);
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    rc = dav_delete_path(r->connection->log, &path, root_len, is_dir);
    if (rc == NGX_OK) {
        update_anchor_ine(&r->headers_in.user);
        return NGX_HTTP_NO_CONTENT;
    }
    // add by james 20161115
    unlock_space_ine(&r->headers_in.user);
    return rc;
}

//
static ngx_int_t ngx_http_dav_mkcol_handler(ngx_http_request_t *r, ngx_http_dav_loc_conf_t *dlcf) {
    u_char *last;
    size_t root_len;
    ngx_str_t path;
//
    if (r->headers_in.content_length_n > 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: MKCOL with body is unsupported");
        return NGX_HTTP_UNSUPPORTED_MEDIA_TYPE;
    }
    /* if (r->uri.data[r->uri.len - 1] != '/') { // disable by james 20161120 for client bug
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: MKCOL can create a collection only");
        return NGX_HTTP_CONFLICT;
    } */
    last = ngx_http_map_uri_to_path(r, &path, &root_len, 0);
    if (last == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    *(last - 1) = '\0';
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http mkcol path: \"%s\"", path.data);
    if (ngx_create_dir(path.data, ngx_dir_access(dlcf->access))
            != NGX_FILE_ERROR) {
        if (ngx_http_dav_location(r, path.data) != NGX_OK) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
        return NGX_HTTP_CREATED;
    }
//
    return dav_http_errno(r->connection->log, ngx_errno,
            NGX_HTTP_CONFLICT, ngx_create_dir_n, path.data);
}

//
static ngx_int_t dav_move_dir_pre(tree_ctx_t *ctx, ngx_str_t *path) {
    u_char *last, *dir;
    ngx_http_dav_mvcp_ctx_t *mctx;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http move dir: \"%s\"", path->data);
    mctx = ctx->data;
    dir = ngx_alloc(mctx->path.len + path->len + 1, ctx->log);
    if (dir == NULL) {
        return NGX_ABORT;
    }
    last = ngx_cpymem(dir, mctx->path.data, mctx->path.len);
    (void) ngx_cpystrn(last, path->data + mctx->len, path->len - mctx->len + 1);
    // add by james 20161115
    ngx_log_error(NGX_LOG_ALERT, ctx->log, 0, "DAV: http create dir: \"%s\"", dir);
    if (ngx_create_dir(dir, ngx_dir_access(ctx->access)) == NGX_FILE_ERROR) {
        (void) dav_http_errno(ctx->log, ngx_errno, 0, ngx_create_dir_n, dir);
    }
    if (ngx_set_file_time(dir, 0, ctx->mtime) != NGX_OK) {
        ngx_log_error(NGX_LOG_ALERT, ctx->log, ngx_errno, ngx_set_file_time_n " \"%s\" failed", dir);
    }
    ngx_free(dir);
//
    return NGX_OK;
}

static ngx_int_t dav_move_dir_post(tree_ctx_t *ctx, ngx_str_t *path) {
    if (ngx_delete_dir(path->data) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, ctx->log, ngx_errno, ngx_delete_dir_n"DAV: \"%s\" failed", path->data);
    }
    return NGX_OK;
}

static ngx_int_t dav_move_file(tree_ctx_t *ctx, ngx_str_t *path, size_t root_len) {
    u_char *last, *file;
    ngx_copy_file_t cf;
    ngx_http_dav_mvcp_ctx_t *mctx;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http move file: \"%s\"", path->data);
    mctx = ctx->data;
    file = ngx_alloc(mctx->path.len + path->len + 1, ctx->log);
    if (file == NULL) {
        return NGX_ABORT;
    }
    last = ngx_cpymem(file, mctx->path.data, mctx->path.len);
    (void) ngx_cpystrn(last, path->data + mctx->len, path->len - mctx->len + 1);
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http move file to: \"%s\"", file);
    if (ngx_rename_file(path->data, file) != NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "Cloud: move_insert_ine: |%s| |%s|", path->data + root_len, file + root_len);
        move_insert_ine(path->data + root_len, file + root_len)
        LINK_KALIVE
        ngx_free(file);
        return NGX_OK;
    }
    //
    cf.size = ctx->size;
    cf.buf_size = 0;
    cf.access = ctx->access;
    cf.time = ctx->mtime;
    cf.log = ctx->log;
    (void) ngx_copy_file(path->data, file, &cf);
    if (ngx_delete_file(path->data) == NGX_FILE_ERROR) {
        ngx_log_error(NGX_LOG_ALERT, ctx->log, ngx_errno, ngx_delete_file_n"DAV: \"%s\" failed", file);
    }
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "Cloud: move_insert_ine: |%s| |%s|", path->data + root_len, file + root_len);
    move_insert_ine(path->data + root_len, file + root_len)
    ngx_free(file);
    DATA_KALIVE
//
    return NGX_OK;
}

static u_char *dav_uri_to_path(ngx_pool_t *pool, ngx_str_t *dest, ngx_str_t *exis, size_t root_len, ngx_str_t *dest_uri) {
    dest->len = dest_uri->len + exis->len - root_len;
    dest->data = ngx_pnalloc(pool, dest->len + 0x01);
    if (dest->data == NULL) return NULL;
    u_char *last = ngx_copy(dest->data, exis->data, exis->len - root_len);
    last = ngx_cpystrn(last, dest_uri->data, dest_uri->len + 0x01);
    return last;
}

static ngx_int_t ngx_http_dav_move_handler(ngx_http_request_t *r) {
    u_char *line, *host, *last, ch;
    size_t len, root_len;
    ngx_int_t rc, depth;
    ngx_uint_t overwrite, is_dir, flags;
    ngx_str_t path, dest_uri, args;
    ngx_file_info_t fi;
    ngx_table_elt_t *dest, *over;
    ngx_http_dav_mvcp_ctx_t mctx;
//
    if (r->headers_in.content_length_n > 0) {
        return NGX_HTTP_UNSUPPORTED_MEDIA_TYPE;
    }
    dest = r->headers_in.destination;
    if (dest == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent no \"Destination\" header");
        return NGX_HTTP_BAD_REQUEST;
    }
    line = dest->value.data;
    /* there is always '\0' even after empty header value */
    if (line[0] == '/') {
        last = line + dest->value.len;
        goto destination_done;
    }
    len = r->headers_in.server.len;
    if (len == 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent no \"Host\" header");
        return NGX_HTTP_BAD_REQUEST;
    }
#if (NGX_HTTP_SSL)
    if (r->connection->ssl) {
        if (ngx_strncmp(dest->value.data, "https://", sizeof ("https://") - 1)
                != 0) {
            goto invalid_destination;
        }
        host = dest->value.data + sizeof ("https://") - 1;
    } else
#endif
    {
        if (ngx_strncmp(dest->value.data, "http://", sizeof ("http://") - 1) != 0) {
            goto invalid_destination;
        }
        host = dest->value.data + sizeof ("http://") - 1;
    }
    if (ngx_strncmp(host, r->headers_in.server.data, len) != 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Destination\" URI \"%V\" is handled by "
                "different repository than the source URI", &dest->value);
        return NGX_HTTP_BAD_REQUEST;
    }
    last = dest->value.data + dest->value.len;
    for (line = host + len; line < last; line++) {
        if (*line == '/') {
            goto destination_done;
        }
    }
invalid_destination:
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
            "DAV: client sent invalid \"Destination\" header: \"%V\"", &dest->value);
    return NGX_HTTP_BAD_REQUEST;
destination_done:
    dest_uri.len = last - line;
    dest_uri.data = line;
    flags = NGX_HTTP_LOG_UNSAFE;
    if (ngx_http_parse_unsafe_uri(r, &dest_uri, &args, &flags) != NGX_OK) {
        goto invalid_destination;
    }
    if ((r->uri.data[r->uri.len - 1]=='/' && *(last - 1)!='/') || (r->uri.data[r->uri.len - 1]!='/' && *(last - 1)=='/')) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: both URI \"%V\" and \"Destination\" URI \"%V\" "
                "should be either collections or non-collections", &r->uri, &dest->value);
        return NGX_HTTP_CONFLICT;
    }
    depth = dav_path_depth(r, NGX_HTTP_DAV_INFINITY_DEPTH);
    if (depth != NGX_HTTP_DAV_INFINITY_DEPTH) {
        if (r->method == NGX_HTTP_COPY) {
            if (depth != 0) {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be 0 or infinity");
                return NGX_HTTP_BAD_REQUEST;
            }
        } else {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be infinity");
            return NGX_HTTP_BAD_REQUEST;
        }
    }
    over = r->headers_in.overwrite;
    if (over) {
        if (over->value.len == 1) {
            ch = over->value.data[0];
            if (ch == 'T' || ch == 't') {
                overwrite = 1;
                goto overwrite_done;
            }
            if (ch == 'F' || ch == 'f') {
                overwrite = 0;
                goto overwrite_done;
            }
        }
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "DAV: client sent invalid \"Overwrite\" header: \"%V\"", &over->value);
        return NGX_HTTP_BAD_REQUEST;
    }
    overwrite = 1;
overwrite_done:
    if (ngx_http_map_uri_to_path(r, &path, &root_len, 0) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    path.len--;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http move from: \"%s\"", path.data);
    if (dav_uri_to_path(r->pool, &mctx.path, &path, r->unparsed_uri.len, &dest_uri) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http move to: \"%s\" :%d", mctx.path.data, mctx.path.len);
    if (ngx_link_info(mctx.path.data, &fi) == NGX_FILE_ERROR) { /* destination does not exist */
        if (errno != NGX_ENOENT) {
            return dav_http_errno(r->connection->log, errno,
                    NGX_HTTP_NOT_FOUND, ngx_link_info_n, mctx.path.data);
        }
        overwrite = 0;
        is_dir = 0;
    } else { /* destination exists */
        if (ngx_is_dir(&fi)) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"%V\" could not be %Ved to collection \"%V\"", &r->uri, &r->method_name, &dest->value);
            return NGX_HTTP_CONFLICT;
        }
        if (!overwrite) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EEXIST, "DAV: \"%s\" could not be created", mctx.path.data);
            return NGX_HTTP_PRECONDITION_FAILED;
        }
        is_dir = ngx_is_dir(&fi);
    }
    if (ngx_link_info(path.data, &fi) == NGX_FILE_ERROR) {
        return dav_http_errno(r->connection->log, ngx_errno, NGX_HTTP_NOT_FOUND, ngx_link_info_n, path.data);
    }
     // add by james 20161115
    root_len = path.len - r->unparsed_uri.len;
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: move headers_in.user.data: %s", r->headers_in.user.data);
    if (locked_space_ine(&r->headers_in.user)) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: \"%s\" lock user space error!", path.data);
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    //
    /* if (ngx_is_dir(&fi)) { // disable by james 20161120 for client bug
        if (r->uri.data[r->uri.len - 1] != '/') {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"%V\" is collection", &r->uri);
            unlock_space_ine(&r->headers_in.user);
            return NGX_HTTP_BAD_REQUEST;
        }
    } */
    if (overwrite) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http delete: \"%s\"", mctx.path.data);
        rc = dav_delete_path(r->connection->log, &mctx.path, root_len, is_dir);
        if (rc != NGX_OK) {
            unlock_space_ine(&r->headers_in.user);
            return rc;
        }
    }
    // add by james 20161115
    tree_ctx_t tree;
    ngx_ext_rename_file_t ext;
    ngx_http_dav_loc_conf_t *dlcf;
    //
    if (ngx_is_dir(&fi)) {
        // path.len -= 2; /* omit "/\0" */
        if (ngx_create_dir(mctx.path.data, ngx_file_access(&fi)) == NGX_FILE_ERROR) {
            unlock_space_ine(&r->headers_in.user);
            return dav_http_errno(r->connection->log, ngx_errno, NGX_HTTP_NOT_FOUND, ngx_create_dir_n, mctx.path.data);
        }
        mctx.len = path.len;
        tree.file_handler = dav_move_file;
        tree.pre_tree_handler = dav_move_dir_pre;
        tree.post_tree_handler = dav_move_dir_post;
        tree.spec_handler = dav_delete_spec;
        tree.data = &mctx;
        tree.log = r->connection->log;
        if (dav_walk_tree(&tree, &path, root_len) == NGX_OK) {
            if (ngx_delete_dir(path.data) == NGX_FILE_ERROR) {
                ngx_log_error(NGX_LOG_ALERT, r->connection->log, ngx_errno, ngx_delete_dir_n"DAV: \"%s\" failed", path.data);
            }
            update_anchor_ine(&r->headers_in.user);
            return NGX_HTTP_CREATED;
        }
    } else {
        dlcf = ngx_http_get_module_loc_conf(r, ngx_http_dav_module);
        ext.access = 0;
        ext.path_access = dlcf->access;
        ext.time = -1;
        ext.create_path = 1;
        ext.delete_file = 0;
        ext.log = r->connection->log;
        if (ngx_ext_rename_file(&path, &mctx.path, &ext) == NGX_OK) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Cloud: move_update_ine: |%s| |%s|", path.data + root_len, dest_uri.data);
            move_update_ine(&r->headers_in.user, path.data + root_len, dest_uri.data)
            return NGX_HTTP_NO_CONTENT;
        }
    }    
    //
    unlock_space_ine(&r->headers_in.user);
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
}

//
static ngx_int_t dav_copy_dir(tree_ctx_t *ctx, ngx_str_t *path) {
    u_char *last, *dir;
    ngx_http_dav_mvcp_ctx_t *cctx;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http copy dir time: \"%s\"", path->data);
    cctx = ctx->data;
    dir = ngx_alloc(cctx->path.len + path->len + 1, ctx->log);
    if (dir == NULL) {
        return NGX_ABORT;
    }
    last = ngx_cpymem(dir, cctx->path.data, cctx->path.len);
    (void) ngx_cpystrn(last, path->data + cctx->len, path->len - cctx->len + 1);
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http copy dir time to: \"%s\"", dir);
    if (ngx_create_dir(dir, ngx_dir_access(ctx->access)) == NGX_FILE_ERROR) {
        (void) dav_http_errno(ctx->log, ngx_errno, 0, ngx_create_dir_n, dir);
    }
    if (ngx_set_file_time(dir, 0, ctx->mtime) != NGX_OK) {
        ngx_log_error(NGX_LOG_ALERT, ctx->log, ngx_errno, ngx_set_file_time_n "DAV: \"%s\" failed", dir);
    }
    ngx_free(dir);
//
    return NGX_OK;
}

static ngx_int_t dav_copy_file(tree_ctx_t *ctx, ngx_str_t *path, size_t root_len) {
    u_char *last, *file;
    ngx_copy_file_t cf;
    ngx_http_dav_mvcp_ctx_t *cctx;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http copy file: \"%s\"", path->data);
    cctx = ctx->data;
    file = ngx_alloc(cctx->path.len + path->len + 1, ctx->log);
    if (file == NULL) {
        return NGX_ABORT;
    }
    last = ngx_cpymem(file, cctx->path.data, cctx->path.len);
    (void) ngx_cpystrn(last, path->data + cctx->len, path->len - cctx->len + 1);
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: http copy file to: \"%s\"", file);
    cf.size = ctx->size;
    cf.buf_size = 0;
    cf.access = ctx->access;
    cf.time = ctx->mtime;
    cf.log = ctx->log;
    (void) ngx_copy_file(path->data, file, &cf);
    ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "Cloud : copy_insert_ine: |%s| |%s|", path->data + root_len, file + root_len);
    copy_insert_ine(path->data + root_len, file + root_len, ctx->size)
    ngx_free(file);
    DATA_KALIVE
//
    return NGX_OK;
}

static ngx_int_t ngx_http_dav_copy_handler(ngx_http_request_t *r) {
    u_char *line, *host, *last, ch;
    size_t len, root_len;
    ngx_int_t rc, depth;
    ngx_uint_t overwrite, is_dir, flags;
    ngx_str_t path, dest_uri, args;
    ngx_file_info_t fi;
    ngx_table_elt_t *dest, *over;
    ngx_http_dav_mvcp_ctx_t cctx;
//
    if (r->headers_in.content_length_n > 0) {
        return NGX_HTTP_UNSUPPORTED_MEDIA_TYPE;
    }
    dest = r->headers_in.destination;
    if (dest == NULL) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent no \"Destination\" header");
        return NGX_HTTP_BAD_REQUEST;
    }
    line = dest->value.data;
    /* there is always '\0' even after empty header value */
    if (line[0] == '/') {
        last = line + dest->value.len;
        goto destination_done;
    }
    len = r->headers_in.server.len;
    if (len == 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent no \"Host\" header");
        return NGX_HTTP_BAD_REQUEST;
    }
#if (NGX_HTTP_SSL)
    if (r->connection->ssl) {
        if (ngx_strncmp(dest->value.data, "https://", sizeof ("https://") - 1)
                != 0) {
            goto invalid_destination;
        }
        host = dest->value.data + sizeof ("https://") - 1;
    } else
#endif
    {
        if (ngx_strncmp(dest->value.data, "http://", sizeof ("http://") - 1) != 0) {
            goto invalid_destination;
        }
        host = dest->value.data + sizeof ("http://") - 1;
    }
    if (ngx_strncmp(host, r->headers_in.server.data, len) != 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Destination\" URI \"%V\" is handled by "
                "different repository than the source URI", &dest->value);
        return NGX_HTTP_BAD_REQUEST;
    }
    last = dest->value.data + dest->value.len;
    for (line = host + len; line < last; line++) {
        if (*line == '/') {
            goto destination_done;
        }
    }
invalid_destination:
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent invalid \"Destination\" header: \"%V\"", &dest->value);
    return NGX_HTTP_BAD_REQUEST;
destination_done:
    dest_uri.len = last - line;
    dest_uri.data = line;
    flags = NGX_HTTP_LOG_UNSAFE;
    if (ngx_http_parse_unsafe_uri(r, &dest_uri, &args, &flags) != NGX_OK) {
        goto invalid_destination;
    }
    if ((r->uri.data[r->uri.len - 1]=='/' && *(last - 1)!='/') || (r->uri.data[r->uri.len - 1]!='/' && *(last - 1)=='/')) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: both URI \"%V\" and \"Destination\" URI \"%V\" "
                "should be either collections or non-collections", &r->uri, &dest->value);
        return NGX_HTTP_CONFLICT;
    }
    depth = dav_path_depth(r, NGX_HTTP_DAV_INFINITY_DEPTH);
    if (depth != NGX_HTTP_DAV_INFINITY_DEPTH) {
        if (r->method == NGX_HTTP_COPY) {
            if (depth != 0) {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be 0 or infinity");
                return NGX_HTTP_BAD_REQUEST;
            }
        } else {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"Depth\" header must be infinity");
            return NGX_HTTP_BAD_REQUEST;
        }
    }
    over = r->headers_in.overwrite;
    if (over) {
        if (over->value.len == 1) {
            ch = over->value.data[0];
            if (ch == 'T' || ch == 't') {
                overwrite = 1;
                goto overwrite_done;
            }
            if (ch == 'F' || ch == 'f') {
                overwrite = 0;
                goto overwrite_done;
            }
        }
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: client sent invalid \"Overwrite\" header: \"%V\"", &over->value);
        return NGX_HTTP_BAD_REQUEST;
    }
    overwrite = 1;
overwrite_done:
    if (ngx_http_map_uri_to_path(r, &path, &root_len, 0) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    path.len--;
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http copy from: \"%s\"", path.data);
    if (dav_uri_to_path(r->pool, &cctx.path, &path, r->unparsed_uri.len, &dest_uri) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http copy to: \"%s\"", cctx.path.data);
    if (ngx_link_info(cctx.path.data, &fi) == NGX_FILE_ERROR) { /* destination does not exist */
        if (errno != NGX_ENOENT) {
            return dav_http_errno(r->connection->log, errno,
                    NGX_HTTP_NOT_FOUND, ngx_link_info_n, cctx.path.data);
        }
        overwrite = 0;
        is_dir = 0;
    } else { /* destination exists */
        if (ngx_is_dir(&fi)) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"%V\" could not be %Ved to collection \"%V\"", &r->uri, &r->method_name, &dest->value);
            return NGX_HTTP_CONFLICT;
        }
        if (!overwrite) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EEXIST, "DAV: \"%s\" could not be created", cctx.path.data);
            return NGX_HTTP_PRECONDITION_FAILED;
        }
        is_dir = ngx_is_dir(&fi);
    }
    if (ngx_link_info(path.data, &fi) == NGX_FILE_ERROR) {
        return dav_http_errno(r->connection->log, ngx_errno,
                NGX_HTTP_NOT_FOUND, ngx_link_info_n, path.data);
    }
     // add by james 20161115
    root_len = path.len - r->unparsed_uri.len;
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV copy headers_in.user.data: %s", r->headers_in.user.data);
    if ((rc = increm_locked_ine(&r->headers_in.user))) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, NGX_EISDIR, "DAV: \"%s\" lock user space error!", path.data);
        unlock_space_ine(&r->headers_in.user);
        if(0x01 == rc) return NGX_HTTP_INSUFFICIENT_STORAGE;
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    //
    /* if (ngx_is_dir(&fi)) { // disable by james 20161120 for client bug
        if (r->uri.data[r->uri.len - 1] != '/') {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: \"%V\" is collection", &r->uri);
            unlock_space_ine(&r->headers_in.user);
            return NGX_HTTP_BAD_REQUEST;
        }
    } */
    if (overwrite) { // add by james 20161115
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: http delete: \"%s\"", cctx.path.data);
        rc = dav_delete_path(r->connection->log, &cctx.path, root_len, is_dir);
        if (rc != NGX_OK) {
            unlock_space_ine(&r->headers_in.user);
            return rc;
        }
    }
//
    tree_ctx_t tree;
    ngx_copy_file_t cf;
    ngx_http_dav_loc_conf_t *dlcf;
    if (ngx_is_dir(&fi)) {
        // path.len -= 2; /* omit "/\0" */
        if (ngx_create_dir(cctx.path.data, ngx_file_access(&fi)) == NGX_FILE_ERROR) {
            unlock_space_ine(&r->headers_in.user);
            return dav_http_errno(r->connection->log, ngx_errno, NGX_HTTP_NOT_FOUND, ngx_create_dir_n, cctx.path.data);
        }
        cctx.len = path.len;
        tree.file_handler = dav_copy_file;
        tree.pre_tree_handler = dav_copy_dir;
        tree.post_tree_handler = dav_tree_noop;
        tree.spec_handler = dav_file_noop;
        tree.data = &cctx;
        tree.log = r->connection->log;
        if (dav_walk_tree(&tree, &path, root_len) == NGX_OK) {
            update_anchor_ine(&r->headers_in.user);
            return NGX_HTTP_CREATED;
        }
    } else {
        dlcf = ngx_http_get_module_loc_conf(r, ngx_http_dav_module);
        cf.size = ngx_file_size(&fi);
        cf.buf_size = 0;
        cf.access = dlcf->access;
        cf.time = ngx_file_mtime(&fi);
        cf.log = r->connection->log;
        if (ngx_copy_file(path.data, cctx.path.data, &cf) == NGX_OK) {
            uint64 file_size;
            FILE_SIZE(file_size, cctx.path.data, r->connection->log)
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "Cloud: copy_update_ine: |%s| |%s|", path.data + root_len, dest_uri.data);
            copy_update_ine(&r->headers_in.user, path.data + root_len, dest_uri.data, file_size)
            return NGX_HTTP_NO_CONTENT;
        }
    }
    // add by james 20161115
    unlock_space_ine(&r->headers_in.user);
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
}

//
static ngx_int_t ngx_http_dav_location(ngx_http_request_t *r, u_char *path) {
    u_char *location;
    ngx_http_core_loc_conf_t *clcf;
    //
    r->headers_out.location = ngx_palloc(r->pool, sizeof(ngx_table_elt_t));
    if (r->headers_out.location == NULL) {
        return NGX_ERROR;
    }
    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
    if (!clcf->alias && clcf->root_lengths == NULL) {
        location = path + clcf->root.len;
    } else {
        location = ngx_pnalloc(r->pool, r->uri.len);
        if (location == NULL) {
            return NGX_ERROR;
        }
        ngx_memcpy(location, r->uri.data, r->uri.len);
    }
    /* we do not need to set the r->headers_out.location->hash and
     * r->headers_out.location->key fields */
    r->headers_out.location->value.len = r->uri.len;
    r->headers_out.location->value.data = location;
    return NGX_OK;
}

static void *ngx_http_dav_create_loc_conf(ngx_conf_t *cf) {
    ngx_http_dav_loc_conf_t *conf;
//
    conf = ngx_pcalloc(cf->pool, sizeof (ngx_http_dav_loc_conf_t));
    if (conf == NULL) {
        return NULL;
    }
    /* set by ngx_pcalloc():
     *     conf->methods = 0; */
    conf->min_delete_depth = NGX_CONF_UNSET_UINT;
    conf->access = NGX_CONF_UNSET_UINT;
    conf->create_full_put_path = NGX_CONF_UNSET;
//
    return conf;
}

static char *ngx_http_dav_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {
    ngx_http_dav_loc_conf_t *prev = parent;
    ngx_http_dav_loc_conf_t *conf = child;
//
    ngx_conf_merge_bitmask_value(conf->methods, prev->methods,
            (NGX_CONF_BITMASK_SET | NGX_HTTP_DAV_OFF));
    ngx_conf_merge_uint_value(conf->min_delete_depth,
            prev->min_delete_depth, 0);
    ngx_conf_merge_uint_value(conf->access, prev->access, 0600);
    ngx_conf_merge_value(conf->create_full_put_path,
            prev->create_full_put_path, 0);
//
    return NGX_CONF_OK;
}

static ngx_int_t dav_cloud_init(ngx_conf_t *cf) {
    if(init_enviro_ine()) return NGX_ERROR;
    return NGX_OK;
}

static ngx_int_t ngx_http_dav_init(ngx_conf_t *cf) {
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;
//
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }
    *h = ngx_http_dav_handler;
//
    return NGX_OK;
}

#define dot_directory_con(name, len, dir) \
    len = ngx_de_namelen(dir); \
    name = ngx_de_name(dir); \
    if (0x01==len && name[0]=='.') continue; \
    if (0x02==len && name[0]=='.' && name[1]=='.') continue;

static ngx_int_t dav_walk_tree(tree_ctx_t *ctx, ngx_str_t *tree, size_t root_len) {
    u_char *last, *name;
    size_t len;
    ngx_int_t rc;
    ngx_str_t file, buf;
    ngx_dir_t dir;
    // ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: walk tree \"%V\"", tree); // add by james 20161115
    ngx_str_null(&buf);
    if (ngx_open_dir(tree, &dir) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_CRIT, ctx->log, ngx_errno, ngx_open_dir_n "DAV: \"%s\" failed", tree->data);
        return NGX_ERROR;
    }
    for (;;) {
        if (ngx_read_dir(&dir) == NGX_ERROR) {
            rc = NGX_OK;
            goto done;
        }
        ngx_log_debug2(NGX_LOG_DEBUG_CORE, ctx->log, 0, "DAV: tree name %uz:\"%s\"", len, name);
        dot_directory_con(name, len, &dir)
        file.len = tree->len + 1 + len;
        if (file.len + NGX_DIR_MASK_LEN > buf.len) {
            if (buf.len) {
                ngx_free(buf.data);
            }
            buf.len = tree->len + 1 + len + NGX_DIR_MASK_LEN;
            buf.data = ngx_alloc(buf.len + 1, ctx->log);
            if (buf.data == NULL) {
                goto failed;
            }
        }
//
        last = ngx_cpymem(buf.data, tree->data, tree->len);
        *last++ = '/';
        ngx_memcpy(last, name, len + 1);
        file.data = buf.data;
        // ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: tree path \"%s\"", file.data); // add by james 20161115
        if (!dir.valid_info) {
            if (ngx_de_info(file.data, &dir) == NGX_FILE_ERROR) {
                ngx_log_error(NGX_LOG_CRIT, ctx->log, ngx_errno, ngx_de_info_n " \"%s\" failed", file.data);
                continue;
            }
        }
        if (ngx_de_is_file(&dir)) {
            // ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: tree file \"%s\"", file.data); // add by james 20161115
            ctx->size = ngx_de_size(&dir);
            ctx->access = ngx_de_access(&dir);
            ctx->mtime = ngx_de_mtime(&dir);
            if (ctx->file_handler(ctx, &file, root_len) == NGX_ABORT) {
                goto failed;
            }
        } else if (ngx_de_is_dir(&dir)) {
            // ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: tree enter dir \"%s\"", file.data); // add by james 20161115
            ctx->access = ngx_de_access(&dir);
            ctx->mtime = ngx_de_mtime(&dir);
            if (ctx->pre_tree_handler(ctx, &file) == NGX_ABORT) {
                goto failed;
            }
            if (dav_walk_tree(ctx, &file, root_len) == NGX_ABORT) {
                goto failed;
            }
            ctx->access = ngx_de_access(&dir);
            ctx->mtime = ngx_de_mtime(&dir);
            if (ctx->post_tree_handler(ctx, &file) == NGX_ABORT) {
                goto failed;
            }
        } else {
            // ngx_log_error(NGX_LOG_ERR, ctx->log, 0, "DAV: tree special \"%s\"", file.data); // add by james 20161115
            if (ctx->spec_handler(ctx, &file, root_len) == NGX_ABORT) {
                goto failed;
            }
        }
    }
failed:
    rc = NGX_ABORT;
done:
    if (buf.len) {
        ngx_free(buf.data);
    }
    if (ngx_close_dir(&dir) == NGX_ERROR) {
        ngx_log_error(NGX_LOG_CRIT, ctx->log, ngx_errno, ngx_close_dir_n "DAV: \"%s\" failed", tree->data);
    }
//
    return rc;
}
