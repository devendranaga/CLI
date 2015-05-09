#include "cli_main.h"

int main(int argc, char *argv[])
{
    int ret;
    struct cli_context *cctx;

    cctx = calloc(1, sizeof(*cctx));
    if (!cctx)
        return -1;

    cctx->sapi_evlib_ctx = sapi_event_system_init();
    if (!cctx->sapi_evlib_ctx)
        goto err_sys_init;
    ret = cli_net_server_create(cctx, CSERVER_PATH, CSERVER_N_CONNS);
    if (ret < 0)
        goto err_srv_create;

    sapi_event_loop(cctx->sapi_evlib_ctx);

    cli_net_server_destroy(cctx);

    return 0;

err_srv_create:
    sapi_event_system_deinit(cctx->sapi_evlib_ctx);

err_sys_init:
    free(cctx);

    return -1;
}
