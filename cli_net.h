#ifndef __CLI_NET_H__
#define __CLI_NET_H__

int cli_net_server_create(struct cli_context *cctx, char *path, int conns);

void cli_net_server_destroy(struct cli_context *cctx);

#endif
