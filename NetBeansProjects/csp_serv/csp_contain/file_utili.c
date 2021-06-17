/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dlfcn.h>

#include "contain_macro.h"
#include "logger.h"
#include "file_utili.h"
//

char *get_executable_path(char* epath) {
    char exec_path[PATH_MAX];
    int len = readlink("/proc/self/exe", exec_path, PATH_MAX);
    if (0 > len) return NULL;
    exec_path[len] = '\0';
    //
    strcpy(epath, exec_path);
    return epath;
}

char *get_appli_path() {
    static char appli_path[1024];
    if ('\0' == appli_path[0]) {
        if (!get_executable_path(appli_path)) return NULL;
        char *tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        tokep[0] = '\0';
        tokep = strrchr(appli_path, '/');
        if (!tokep) return NULL;
        (tokep)[0] = '\0';
    }
    return appli_path;
}

//

void *open_handler(struct dyli_fun *dlfup, char *lib_path, char *func_name) {
    static char prev_path[MAX_PATH];
    static void *prev_handle;
    //
    if (strcasecmp(prev_path, lib_path)) {
        prev_handle = dlopen(lib_path, RTLD_LAZY);
        if (!prev_handle) {
            _LOG_FATAL("get_handlers dlopen: %s", dlerror());
            return NULL;
        }
        strcpy(prev_path, lib_path);
    }
    //
    dlfup->handle = prev_handle;
    dlfup->funptr = dlsym(prev_handle, func_name);
    if (dlerror() != NULL) {
        _LOG_FATAL("get_handlers dlsym: %s", dlerror());
        return NULL;
    }
    return dlfup;
}

void close_handler(void *handle) {
    static void *prev_handle;
    if (prev_handle != handle) {
        dlclose(handle);
        prev_handle = handle;
    }
}

// trans func name "login.csp" to "login_csp"

char *fill_invoke_fun(char *invoke_csp) {
    char *toke = strrchr(invoke_csp, '.');
    if (!toke) return NULL;
    toke[0] = '_';
    return invoke_csp;
}

int dyli_open_vlist(struct vpa_list *vlist) {
    struct vpa_list *vlistmp = NULL;
    vlistmp = vlist;
    while (vlistmp) {
        if (fill_invoke_fun(vlistmp->csp_name)) return -1;
        if (!open_handler(&vlistmp->dlfup, vlistmp->lib_path, vlistmp->csp_name))
            return -1;
        vlistmp = vlistmp->next;
    }
    //
    return 0x00;
}

void dyli_close_vlist(struct vpa_list *vlist) {
    struct vpa_list *vlistmp = NULL;
    vlistmp = vlist;
    while (vlistmp) {
        close_handler(vlistmp->dlfup.handle);
        vlistmp = vlistmp->next;
    }
    //
    dele_vlist(vlist);
}