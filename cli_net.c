#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cli_main.h"

int cli_net_server_create(struct cli_context *cctx, char *path, int conns)
{
    int cli;

    cctx->srv_sock = sapi_unix_tcp_server_create(path, conns);
    if (cctx->srv_sock < 0)
        return -1;

    // do not go further in the init process until we finish accepting a connection

    cli = accept(cctx->srv_sock, NULL, NULL);
    if (cli < 0)
        goto err_accept;

    cctx->cli_sock = cli;
    sapi_reg_read_event(cli, cctx->sapi_evlib_ctx, cctx,
                        sapi_process_cli_cmd);
    return 0;
err_accept:
    sapi_unix_tcp_server_destroy(cctx->srv_sock);
    return -1;
}

void cli_net_server_destroy(struct cli_context *cctx)
{
    sapi_unreg_read_event(cctx->sapi_evlib_ctx, cctx->srv_sock);
    sapi_unix_tcp_server_destroy(cctx->srv_sock);
    close(cctx->cli_sock);
}
