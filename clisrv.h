#ifndef __CLISRV_H__
#define __CLISRV_H__

#include <cliutil.h>

#define CLI_SRV_SOCK "/tmp/clisrv.sock"
#define CLI_SRV_MAXCONN 4

struct cli_auth_db {
    char *usrname;
    char *passwd;
    int is_admin;
    struct cli_auth_db *next;
};

struct cli_auth_mgr {
    struct cli_auth_db *db;
};

struct cli_service_priv {
    void *libev_magic;
    int cli_service_sock;
    int cli_client;
    struct cli_auth_mgr *auth_mgr;
};

#endif
