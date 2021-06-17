#include "fcgi_stdio.h"
#include <stdlib.h>
#include <string.h> 

#include <mysql.h>
#include "tiny_pool.h"
//

int csp_login_csp(MYSQL *sql_sock, tiny_pool_t *tpool) {
    printf("Content-type: text/plain\r\n\r\n");
    printf("<title>login</title>");
    int i;
    for (i = 1; i < 10; i++) {
        printf("<h1>csp_login</h1>\r\n");
    }
    //
    return i;
}

int show_time_csp(MYSQL *sql_sock, tiny_pool_t *tpool) {
        printf("Content-type: text/plain\r\n\r\n");
        printf("<title>show time</title>");
    int i;
    for (i = 1; i < 10; i++) {
        printf("<h1>show_time</h1>\r\n");
    }
    //
    return i;
}
