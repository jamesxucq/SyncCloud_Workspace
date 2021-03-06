/* File: ngx_http_auth_dav_module.c */
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <dav_cloud.h>

/* Module context data */
typedef struct {
    ngx_str_t passwd;
} ngx_http_auth_dav_ctx_t;

/* DAV authinfo */
/*
typedef struct {
    ngx_str_t username;
    ngx_str_t password;
    ngx_log_t *log;
} ngx_dav_authinfo;
*/

/* Module configuration struct */
typedef struct {
    ngx_str_t realm; /* http basic auth realm */
    ngx_str_t service_name; /* dav service name */
} ngx_http_auth_dav_loc_conf_t;

/* Module handler */
static ngx_int_t ngx_http_auth_dav_handler(ngx_http_request_t *r);

/* Function that authenticates the user -- is the only function that uses DAV */
static ngx_int_t ngx_http_auth_dav_authenticate(ngx_http_request_t *r, ngx_http_auth_dav_ctx_t *ctx, ngx_str_t *passwd, void *conf);
static ngx_int_t ngx_http_auth_dav_set_realm(ngx_http_request_t *r, ngx_str_t *realm);
static void *ngx_http_auth_dav_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_auth_dav_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_auth_dav_init(ngx_conf_t *cf);
static char *ngx_http_auth_dav(ngx_conf_t *cf, void *post, void *data);
static ngx_conf_post_handler_pt ngx_http_auth_dav_p = ngx_http_auth_dav;

static ngx_command_t ngx_http_auth_dav_commands[] = {
    { ngx_string("auth_dav"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF
        | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_auth_dav_loc_conf_t, realm),
        &ngx_http_auth_dav_p},
    { ngx_string("auth_dav_service"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF
        | NGX_CONF_TAKE1,
        ngx_conf_set_str_slot,
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_auth_dav_loc_conf_t, service_name),
        NULL},
    ngx_null_command
};


static ngx_http_module_t ngx_http_auth_dav_module_ctx = {
    NULL, /* preconfiguration */
    ngx_http_auth_dav_init, /* postconfiguration */
    NULL, /* create main configuration */
    NULL, /* init main configuration */
    NULL, /* create server configuration */
    NULL, /* merge server configuration */
    ngx_http_auth_dav_create_loc_conf, /* create location configuration */
    ngx_http_auth_dav_merge_loc_conf /* merge location configuration */
};


ngx_module_t ngx_http_auth_dav_module = {
    NGX_MODULE_V1,
    &ngx_http_auth_dav_module_ctx, /* module context */
    ngx_http_auth_dav_commands, /* module directives */
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

static ngx_int_t ngx_http_auth_dav_handler(ngx_http_request_t *r) {
    ngx_int_t rc;
    ngx_http_auth_dav_ctx_t *ctx;
    ngx_http_auth_dav_loc_conf_t *alcf;
//
    alcf = ngx_http_get_module_loc_conf(r, ngx_http_auth_dav_module);
    if (alcf->realm.len == 0) {
        return NGX_DECLINED;
    }
    ctx = ngx_http_get_module_ctx(r, ngx_http_auth_dav_module);
    if (ctx) {
        return ngx_http_auth_dav_authenticate(r, ctx, &ctx->passwd, alcf);
    }
    /* Decode http auth user and passwd, leaving values on the request */
    rc = ngx_http_auth_basic_user(r);
    if (rc == NGX_DECLINED) {
        return ngx_http_auth_dav_set_realm(r, &alcf->realm);
    }
    if (rc == NGX_ERROR) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
    /* Check user & password using DAV */
    return ngx_http_auth_dav_authenticate(r, ctx, &ctx->passwd, alcf);
}

static ngx_int_t ngx_http_auth_dav_authenticate(ngx_http_request_t *r, ngx_http_auth_dav_ctx_t *ctx, ngx_str_t *passwd, void *conf) {
    ngx_int_t rc;
    ngx_http_auth_dav_loc_conf_t *alcf;
    alcf = conf;

    /* Get username and password, note that r->headers_in.user contains the
     * string 'user:pass', so we need to copy the username */
    /* Initialize DAV */
    // add by james 20161115
    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: Auth: headers_in.user.data: %s", r->headers_in.user.data);
    if ((rc = dav_authen_ine(&r->headers_in.user))) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "DAV: user '%s' - not authenticated: %d", r->headers_in.user.data, rc);
        return ngx_http_auth_dav_set_realm(r, &alcf->realm);
    }
    //
    return NGX_OK;
}

static ngx_int_t ngx_http_auth_dav_set_realm(ngx_http_request_t *r, ngx_str_t *realm) {
    r->headers_out.www_authenticate = ngx_list_push(&r->headers_out.headers);
    if (r->headers_out.www_authenticate == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }
//
    r->headers_out.www_authenticate->hash = 1;
    r->headers_out.www_authenticate->key.len = sizeof ("WWW-Authenticate") - 1;
    r->headers_out.www_authenticate->key.data = (u_char *) "WWW-Authenticate";
    r->headers_out.www_authenticate->value = *realm;
//
    return NGX_HTTP_UNAUTHORIZED;
}

static void *ngx_http_auth_dav_create_loc_conf(ngx_conf_t *cf) {
    ngx_http_auth_dav_loc_conf_t *conf;
    conf = ngx_pcalloc(cf->pool, sizeof (ngx_http_auth_dav_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    return conf;
}

static char *ngx_http_auth_dav_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child) {
    ngx_http_auth_dav_loc_conf_t *prev = parent;
    ngx_http_auth_dav_loc_conf_t *conf = child;
//
    if (conf->realm.data == NULL) {
        conf->realm = prev->realm;
    }
    if (conf->service_name.data == NULL) {
        conf->service_name = prev->service_name;
    }
//
    return NGX_CONF_OK;
}

static ngx_int_t ngx_http_auth_dav_init(ngx_conf_t *cf) {
    ngx_http_handler_pt *h;
    ngx_http_core_main_conf_t *cmcf;
//
    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);
    h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }
    *h = ngx_http_auth_dav_handler;
    return NGX_OK;
}

static char *ngx_http_auth_dav(ngx_conf_t *cf, void *post, void *data) {
    ngx_str_t *realm = data;
    size_t len;
    u_char *basic, *p;
//
    if (ngx_strcmp(realm->data, "off") == 0) {
        realm->len = 0;
        realm->data = (u_char *) "";
        return NGX_CONF_OK;
    }
    len = sizeof ("Basic realm=\"") - 1 + realm->len + 1;
    basic = ngx_palloc(cf->pool, len);
    if (basic == NULL) {
        return NGX_CONF_ERROR;
    }
    p = ngx_cpymem(basic, "Basic realm=\"", sizeof ("Basic realm=\"") - 1);
    p = ngx_cpymem(p, realm->data, realm->len);
    *p = '"';
    realm->len = len;
    realm->data = basic;
//
    return NGX_CONF_OK;
}

/* File: ngx_http_auth_dav_module.c */