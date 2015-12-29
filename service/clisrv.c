#include "libev.h"
#include "cliintf.h"
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

static int __libev_exec_command_show_cli_version(char *version,
                                                 int version_len)
{
    strncpy(version, "cli version beta", version_len);
    return strlen(version) + 1;
}

static int __libev_exec_command_show_date(char *date, int date_len)
{
    time_t now;
    struct tm *t;
    int ret;

    now = time(0);
    t = gmtime(&now);
    if (t) {
        ret = snprintf(date, date_len,
                 "%02d/%02d/%04d-%02d:%02d:%02d",
                 t->tm_mon + 1,
                 t->tm_mday,
                 t->tm_year + 1900,
                 t->tm_hour,
                 t->tm_min,
                 t->tm_sec);
    } else
        ret = -1;

    return ret;
}

void libev_client_data_recv(int sock, void *app_arg)
{
    struct cli_service_priv *priv = app_arg;
    struct cli_interface *intf;
    struct cli_interface_cmdreq *req;
    uint8_t data[1000];
    int len;

    len = recv(sock, data, sizeof(data), 0);
    if (len <= 0) {
        libev_unregister_sock(sock, priv->libev_magic);
        close(sock);
    }

    intf = (struct cli_interface *)data;

    switch (intf->type) {
        case CLI_INTF_CMD_REQ: {
            req = (struct cli_interface_cmdreq *)intf->data;

            switch (req->command) {
                case CLI_COMMAND_SHOW: {
                    switch (req->sub_command) {
                        case CLI_SUBCOMMAND_SHOW_DATE: {
                        }
                    }
                }
            }
        }
    }
}

void libev_service_client(int sock, void *app_arg)
{
    struct cli_service_priv *priv = app_arg;

    priv->cli_client = accept(priv->cli_service_sock, NULL, NULL);
    if (priv->cli_client < 0)
        return;

    fprintf(stderr, "client connected %d\n", priv->cli_client);
    
    libev_register_sock(priv->cli_client,
                        priv->libev_magic,
                        priv, libev_client_data_recv);
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

    priv->cli_service_sock = libev_unix_tcp_init(priv->libev_magic,
                                                 CLI_SRV_SOCK,
                                                 libev_service_client, priv);
    if (priv->cli_service_sock < 0)
        return -1;

    libev_main(priv->libev_magic);

    return 0;
}
