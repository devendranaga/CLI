#include "libev.h"
#include "libnet.h"
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

int cli_service_send_response(int sock,
                              CliCommands_t cmd,
                              CliSubCommands_t subcmd,
                              CliCommandRes_t result,
                              char *data, int len)
{
    uint8_t buf[1000];
    struct cli_interface *intf;
    struct cli_interface_cmdresp *resp;

    intf = (struct cli_interface *)buf;
    resp = (struct cli_interface_cmdresp *)intf->data;

    resp->command = cmd;
    resp->sub_command = subcmd;
    resp->res = result;
    resp->datalen = len;
    memcpy(resp->data, data, len);

    intf->len = sizeof(struct cli_interface_cmdresp) + len;

    return send(sock, buf, sizeof(*intf) + intf->len, 0);
}

static int __cli_service_exec_command_show_cli_version(char *version,
                                                 int version_len)
{
    strncpy(version, "cli version beta", version_len);
    return strlen(version) + 1;
}

static int __cli_service_exec_command_show_date(char *date, int date_len)
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

static int __cli_service_show_date(int sock, CliCommands_t cmd,
                                   CliSubCommands_t subcmd,
                                   struct cli_service_priv *priv)
{
    char exec_buf[100];
    int ret_len;
    CliCommandRes_t res;

    ret_len = __cli_service_exec_command_show_date(exec_buf,
                                                   sizeof(exec_buf));
    if (ret_len == -1)
        res = CLI_COMMAND_RES_FAIL;
    else
        res = CLI_COMMAND_RES_SUCCESS;

    return cli_service_send_response(sock, cmd, subcmd, res, exec_buf, ret_len);
}

static int __cli_service_show_cli_version(int sock, CliCommands_t cmd,
                                          CliSubCommands_t subcmd,
                                          struct cli_service_priv *priv)
{
    char exec_buf[100];
    int ret_len;
    CliCommandRes_t res;

    ret_len = __cli_service_exec_command_show_cli_version(exec_buf,
                                                          sizeof(exec_buf));
    if (ret_len == -1)
        res = CLI_COMMAND_RES_FAIL;
    else
        res = CLI_COMMAND_RES_SUCCESS;

    return cli_service_send_response(sock, cmd, subcmd, res, exec_buf, ret_len);
}

static int __cli_service_show_if_ip(int sock, CliCommands_t cmd,
                                    CliSubCommands_t subcmd,
                                    uint8_t *data, int datalen,
                                    struct cli_service_priv *priv)
{
    int ret;
    char ip[100];
    CliCommandRes_t res;

    printf("%s %u\n", __func__, __LINE__);
    ret = libnet_get_if_ipv4(priv, ip, data);
    if (ret)
        res = CLI_COMMAND_RES_FAIL;
    else
        res = CLI_COMMAND_RES_SUCCESS;

    printf("interface %s ip %s\n", data, ip);

    return cli_service_send_response(sock, cmd, subcmd, res, ip, strlen(ip) + 1);
}

static int __cli_service_show_interfaces(int sock, CliCommands_t cmd,
                                         CliSubCommands_t subcmd,
                                         struct cli_service_priv *priv)
{
    struct libnet_if *ifi = NULL, *ifa;
    CliCommandRes_t res;
    int ret;
    int len = 0, off = 0;
    char buff[1000];

    ret = libnet_get_all_ifs(NULL, &ifi);
    if (ret) {
        res = CLI_COMMAND_RES_FAIL;
        goto err_send;
    }

    memset(buff, 0, sizeof(buff));

    off += 4;
    for (ifa = ifi; ifa; ifa = ifa->next) {
        struct cli_iflist ifitem;

        strcpy(ifitem.ifname, ifa->ifname);
        memcpy(buff + off, &ifitem, sizeof(struct cli_iflist));
        off += sizeof(struct cli_iflist);
        len++;
    }

    memcpy(&buff[0], &len, sizeof(int));

err_send:
    return cli_service_send_response(sock, cmd, subcmd, res, buff, off);
}

void libev_client_data_recv(int sock, void *app_arg)
{
    struct cli_service_priv *priv = app_arg;
    struct cli_interface *intf;
    struct cli_interface_cmdreq *req;
    uint8_t data[1000];
    int len;

    memset(data, 0, sizeof(data));

    len = recv(sock, data, sizeof(data), 0);
    if (len <= 0) {
        libev_unregister_sock(sock, priv->libev_magic);
        close(sock);
        priv->cli_client = -1;
        // no more client process.. let us stop because
        // we do not support multi-clients
        exit(1);
    }

    intf = (struct cli_interface *)data;

    switch (intf->type) {
        case CLI_INTF_CMD_REQ: {
            req = (struct cli_interface_cmdreq *)intf->data;

            switch (req->command) {
                case CLI_COMMAND_SHOW: {
                    switch (req->sub_command) {
                        case CLI_SUBCOMMAND_SHOW_DATE: {
                            __cli_service_show_date(sock, req->command,
                                                    req->sub_command, priv);
                            break;
                        }
                        case CLI_SUBCOMMAND_SHOW_CLI_VER: {
                            __cli_service_show_cli_version(sock, req->command,
                                                           req->sub_command,
                                                           priv);
                            break;
                        }
                        case CLI_SUBCOMMAND_SHOW_INTERFACES: {
                            __cli_service_show_interfaces(sock, req->command,
                                                          req->sub_command,
                                                          priv);
                            break;
                        }
                        case CLI_SUBCOMMAND_SHOW_IF_IP: {
                            if (req->datalen <= 0) {
                                fprintf(stderr, "server: invalid data length\n");
                                return -1;
                            }
                            __cli_service_show_if_ip(sock, req->command,
                                                     req->sub_command,
                                                     req->data, req->datalen,
                                                     priv);
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case CLI_INTF_CMD_RESP: {
            break;
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
    struct cli_service_priv *priv;

    priv = calloc(1, sizeof(struct cli_service_priv));
    if (!priv) {
        cli_memalloc_err(__FILE__, __func__, __LINE__);
        return -1;
    }

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

