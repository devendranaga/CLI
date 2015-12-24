#include "libev.h"
#include "clisrv.h"

#define CLI_SERVICE_OPTS ""

void cli_service_cmdargs_parse(int argc, char **argv)
{
	int opts;

	while ((opts = getopt(argc, argv, CLI_SERVICE_OPTS)) != -1) {
		switch (opts) {

		}
	}
}

void libev_client_data_recv(void *app_arg)
{

}

void libev_service_client(void *app_arg)
{
	struct cli_service_priv *priv = app_arg;

	priv->cli_client = accept(priv->cli_service_sock, NULL, NULL);
	if (priv->cli_client < 0)
		return;

	libev_register_sock(priv->cli_client, priv->libev_magic, priv, libev_client_data_recv);
}

int main(int argc, char **argv)
{
    int ret;
    struct cli_service_priv *priv;

    priv = calloc(1, sizeof(struct cli_service_priv));
    if (!priv)
    	return -1;

    cli_service_cmdargs_parse(argc, argv);

    priv->libev_magic = libev_system_init();
    if (!priv->libev_magic)
    	return -1;

    priv->cli_service_sock = libev_unix_tcp_init(priv->libev_magic, CLI_SRV_SOCK, libev_service_client, priv);
    if (priv->cli_service_sock < 0)
    	return -1;

    libev_main(priv->libev_magic);

    return 0;
}
