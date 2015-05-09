#ifndef __CLI_MAIN_H__
#define __CLI_MAIN_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sysapi_net.h>
#include <sysapi_time.h>
#include <sysapi_event.h>

struct cli_context {
    int srv_sock;
    int cli_sock;
    void *sapi_evlib_ctx;
};

#include <cli_net.h>

void sapi_process_cli_cmd(void *app_ctx);

#define CSERVER_PATH "/var/csrv.sock"
#define CSERVER_N_CONNS 20

#endif
