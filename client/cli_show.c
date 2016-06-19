#include "cliincludes.h"
#include "cliintf.h"
#include "clicmd.h"
#include "clipriv.h"

void cli_service_show(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_help_show_date(
                 int tab_level,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_help_show_interfaces(
                 int tab_level,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_help_show_cli_version(
                 int tab_level, 
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_help_show(
                 int tab_level,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_help_show_cmd(
                 int tab_level,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_show_date(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_show_interfaces(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_show_cli_version(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv);

void cli_service_show_help(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv);

struct cli_commands show_subcmd[] = {
    {
        "date",
        CLI_PRIV_NO_PRIV,
        cli_service_help_show_date,
        cli_service_show_date,
        NULL,
        0
    },

    {
        "interfaces",
        CLI_PRIV_NO_PRIV,
        cli_service_help_show_interfaces,
        cli_service_show_interfaces,
        NULL,
        0
    },

    {
        "cli_version",
        CLI_PRIV_NO_PRIV,
        cli_service_help_show_cli_version,
        cli_service_show_cli_version,
        NULL,
        0
    },

    {
        "?",
        CLI_PRIV_NO_PRIV,
        cli_service_help_show_cmd,
        cli_service_show_help,
        NULL,
        0,
    },

    {
        "help",
        CLI_PRIV_NO_PRIV,
        cli_service_help_show_cmd,
        cli_service_show_help,
        NULL,
        0,
    },
};

struct cli_commands show_cmd = {
    "show",
    CLI_PRIV_NO_PRIV,
    cli_service_help_show,
    cli_service_show,
    show_subcmd,
    sizeof(show_subcmd) / sizeof(show_subcmd[0])
};

void cli_service_show_date_resp(struct cli_interface_cmdresp *resp,
                                void *priv)
{
    if (resp->res == CLI_COMMAND_RES_FAIL)
        fprintf(stderr, "failed to execute command\n");
    else
        fprintf(stderr, "%s\n", resp->data);
}

void cli_service_show_cliversion_resp(struct cli_interface_cmdresp *resp,
                                      void *priv)
{
    cli_service_show_date_resp(resp, priv);
}

void cli_service_recv_timeout(int timeout,
                              int sock,
                              struct command_subsections *ss,
                              int args_len,
                              struct cli_commands *cmd,
                              void *priv,
                              void (*show_func)(struct cli_interface_cmdresp *resp,
                                                void *priv))
{
    struct timeval tv = {
        .tv_sec = timeout,
        .tv_usec = 0,
    };

    fd_set fds;
    int ret;

    FD_ZERO(&fds);
    FD_SET(sock, &fds);

    ret = select(sock + 1, &fds, NULL, NULL, &tv);
    if (ret == 0) {
        fprintf(stderr, "failed to execute command (timedout)\n");
    } else if (ret > 0) {
        uint8_t buff[1000];

        ret = recv(sock, buff, sizeof(buff), 0);
        if (ret > 0) {
            struct cli_interface *intf;
            struct cli_interface_cmdresp *resp;

            intf = (struct cli_interface *)buff;
            resp = (struct cli_interface_cmdresp *)intf->data;

            if (show_func)
                show_func(resp, priv);
        }
    }
}

void cli_service_show_cli_version(
                         struct command_subsections *ss,
                         int args_len,
                         struct cli_commands *cmd,
                         void *priv
                                 )
{
    int ret;
    struct cli_interface_cmdreq *req;
    struct cli_interface *intf;
    uint8_t buff[1000];

    intf = (struct cli_interface *)buff;

    intf->type = CLI_INTF_CMD_REQ;
    intf->len = sizeof(struct cli_interface_cmdreq);

    req = (struct cli_interface_cmdreq *)intf->data;

    req->command = CLI_COMMAND_SHOW;
    req->sub_command = CLI_SUBCOMMAND_SHOW_CLI_VER;
    req->priv = cmd->priv;

    struct cli_client_priv *context = priv;

    ret = send(context->server_conn, buff, sizeof(*req) + sizeof(*intf), 0);
    if (ret > 0) {
        cli_service_recv_timeout(5, context->server_conn,
                                 ss, args_len, cmd, priv,
                                 cli_service_show_cliversion_resp);
    }
}

void cli_service_show_date(
                         struct command_subsections *ss,
                         int args_len,
                         struct cli_commands *cmd,
                         void *priv
                          )
{
    int ret;
    struct cli_interface_cmdreq *req;
    struct cli_interface *intf;
    uint8_t buff[1000];

    intf = (struct cli_interface *)buff;

    intf->type = CLI_INTF_CMD_REQ;
    intf->len = sizeof(struct cli_interface_cmdreq);

    req = (struct cli_interface_cmdreq *)intf->data;

    req->command = CLI_COMMAND_SHOW;
    req->sub_command = CLI_SUBCOMMAND_SHOW_DATE;
    req->priv = cmd->priv;

    struct cli_client_priv *context = priv;

    ret = send(context->server_conn,
               buff, sizeof(*req) + sizeof(*intf), 0);
    if (ret > 0) {
        cli_service_recv_timeout(5, context->server_conn,
                                 ss, args_len, cmd, priv,
                                 cli_service_show_date_resp);
    }
}

void cli_service_show_interfaces_resp(struct cli_interface_cmdresp *resp,
                                      void *priv)
{
    int i;
    int len;
    int off = 0;

    memcpy(&len, resp->data, sizeof(len));
    off += sizeof(int);

    for (i = 0; i < len; i++) {
        struct cli_iflist *ifi;

        ifi = (struct cli_iflist *)(resp->data + off);

        fprintf(stderr, "\t[%s]\n", ifi->ifname);

        off += sizeof(struct cli_iflist);
    }
}

void cli_service_show_interfaces(
                         struct command_subsections *ss,
                         int args_len,
                         struct cli_commands *cmd,
                         void *priv
                                 )
{
    int ret;
    struct cli_interface_cmdreq *req;
    struct cli_interface *intf;
    uint8_t buff[1000];

    intf = (struct cli_interface *)buff;

    intf->type = CLI_INTF_CMD_REQ;
    intf->len = sizeof(struct cli_interface_cmdreq);

    req = (struct cli_interface_cmdreq *)intf->data;

    req->command = CLI_COMMAND_SHOW;
    req->sub_command = CLI_SUBCOMMAND_SHOW_INTERFACES;
    req->priv = cmd->priv;

    struct cli_client_priv *context = priv;

    ret = send(context->server_conn, buff, sizeof(*req) + sizeof(*intf), 0);
    if (ret > 0) {
        cli_service_recv_timeout(5, context->server_conn,
                                 ss, args_len, cmd, priv,
                                 cli_service_show_interfaces_resp);
    }
}

void cli_service_show(
                     struct command_subsections *ss,
                     int args_len,
                     struct cli_commands *cmd,
                     void *priv
                     )
{
    int i, j;
    int found = 0;
    int len = sizeof(show_subcmd) / sizeof(show_subcmd[0]);

    for (i = 0; i < len; i++) {
        for (j = 0; j < args_len; j++) {
            if (!strcmp(show_subcmd[i].command, ss[j].section)) {
                found = 1;
                struct cli_commands *sc = cmd->sub_cmd;

                sc[i].cmdfunc(ss, args_len - 1, sc, priv);
            }
        }
    }

    if (!found)
        cli_service_help_show(1, &show_cmd, priv);
}

void cli_service_help_show_date(
                         int tab_level,
                         struct cli_commands *cmd,
                         void *priv
                               )
{
    fprintf(stderr, "date \t\t <show the current system date>\n");
}

void cli_service_help_show_interfaces(
                         int tab_level,
                         struct cli_commands *cmd,
                         void *priv
                                      )
{
    fprintf(stderr, "interfaces \t\t <show the network interfaces>\n");
}


void cli_service_help_show_cli_version(
                         int tab_level,
                         struct cli_commands *cmd,
                         void *priv
                                      )
{
    fprintf(stderr, "cli_version \t\t <show the cli version>\n");
}

void cli_service_help_show(int tab_level, struct cli_commands *cmd, void *priv)
{
    fprintf(stderr, "show <sub commands>\n");
    if (cmd->sub_cmd) {
        struct cli_commands *scmd;
        int i;

        scmd = cmd->sub_cmd;
        for (i = 0; i < cmd->sub_cmd_len; i++)
            scmd[i].helpmsg(2, &scmd[i], priv);
    }
}

void cli_service_help_show_cmd(int tab_level, struct cli_commands *cmd, void *priv)
{
    fprintf(stderr, "show %s \t\t <display help for show>\n", cmd->command);
}

void cli_service_show_help(
                 struct command_subsections *ss,
                 int args_len,
                 struct cli_commands *cmd,
                 void *priv)
{
    cli_service_help_show(1, &show_cmd, priv);
}

