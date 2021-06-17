#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "qmap.h"
#include"invoke.h"
#include "qcgi_inte.h"

//
// char _buffer_[PAGE_BUFFER_SIZE];
// qentry_t _request_;
// qentry_init(&_request_, char *out_data);
// _request_.truncate(&_request_);
static char *qcgi_req_tab[] = {
    "qcgireq_setoption_bzl",
    "qcgireq_parse_bzl",
    "qcgires_setcookie_bzl",
    "qcgires_removecookie_bzl",
    "qcgires_setcontenttype_bzl",
    "qcgires_getcontenttype_bzl",
    "qcgires_redirect_bzl",
    "qcgires_download_bzl",
    "qcgisess_init_bzl",
    NULL
};

//
// qentry_t *_sess_ = qcgisess_init(&_request_, NULL);
// _sess_->free(_sess_);
static char *qcgi_sess_tab[] = {
    "qcgisess_init_bzl",
    "qcgisess_settimeout_bzl",
    "qcgisess_getid_bzl",
    "qcgisess_getcreated_bzl",
    "qcgisess_save_bzl",
    "qcgisess_destroy_bzl",
    NULL
};

//
// #undef SET_CONTENT_TYPE
static char *qcgi_contype_tab[] = {
    "qcgires_setcontenttype_bzl",
    "qcgires_download_bzl",
    NULL
};

// #undef BUFFER_AUTO_FLUSH
static char *qcgi_flush_tab[] = {
    "qcgires_download_bzl",
    NULL
};

static char *qcgi_qparse_tab[] = {
    "qcgireq_parse_bzl",
    NULL
};

//
keymap *_req_kmap_;
keymap *_sess_kmap_;
keymap *_contype_kmap_;

int init_qcgi_hmap() {
    _req_kmap_ = build_key_hmap(qcgi_req_tab);
    if(!_req_kmap_) return -1;
    _sess_kmap_ = build_key_hmap(qcgi_sess_tab);
    if(!_sess_kmap_) return -1;
    _contype_kmap_ = build_key_hmap(qcgi_contype_tab);
    if(!_contype_kmap_) return -1;
    //
    return 0x00;
}

void fina_qcig_hmap() {
    destory_key_hmap(_contype_kmap_);
    destory_key_hmap(_sess_kmap_);
    destory_key_hmap(_req_kmap_);
}

unsigned int check_qcig_invoke(char *str) {
    unsigned int fivk_type = FIVK_NULL;
    //
    if(khm_value(str, _req_kmap_)) fivk_type |= FIVK_CONBUFF | FIVK_REQUIRE;
    if(khm_value(str, _sess_kmap_)) fivk_type |= FIVK_SESSION;
    if(khm_value(str, _contype_kmap_)) fivk_type |= FIVK_CONTYPE;
    if(!strcmp(qcgi_flush_tab[0x00], str)) fivk_type |= FIVK_AFLUSH;
    if(!strcmp(qcgi_qparse_tab[0x00], str)) fivk_type |= FIVK_QPARSE;
// printf("check str:|%s| fivk_type:%08X\n", str, fivk_type);
    return fivk_type;
}