/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "fpm_macro.h"
#include "fpm_socket.h"
#include "file_utili.h"
#include "options.h"
//
#define CONST_STR_LEN(s) s, sizeof(s) - 1

#ifdef USE_IPV6
#define PACKAGE_FEATURES " (ipv6)"
#else
#define PACKAGE_FEATURES ""
#endif

#define PACKAGE_VERSION "1.0sc"
#define PACKAGE_DESC "csp-fpm v" PACKAGE_VERSION PACKAGE_FEATURES " - csp FastCGI processes\n"

//
static void show_version() {
    (void) write_all(1, CONST_STR_LEN(PACKAGE_DESC));
}

//
void show_help() {
    (void) write_all(0x01, CONST_STR_LEN(
		" -c <config>   config file path\n" \
		" -v            show version\n" \
		" -?, -h        show this help\n" \
	));
}

//
int parse_args(char *xml_config, int argc, char *argv[]) {
    int o;
    //
    MKZERO(xml_config);
    while (-1 != (o = getopt(argc, argv, "c:v?h"))) {
        switch (o) {
            case 'f':
                strcpy(xml_config, optarg);
                break;
            case 'v': 
                show_version();
                return -1;
            case '?':
            case 'h': 
                show_help();
                return -1;
            default:
                
                break;
        }
    }
    if(!xml_config[0]) CREAT_CONF_PATH(xml_config, CSPFPM_DEFAULT)
    //
    return 0x00;
}
