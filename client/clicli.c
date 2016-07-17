#include "libev.h"
#include "clisrv.h"
#include "cliintf.h"
#include "clicmd.h"
#include "clipriv.h"

#define CLI_CLI_CMDLINE_OPTS "n:"

static char *cli_name = "cli >";

void cli_client_cmdline_opts_parse(
                              int argc,
                              char **argv,
                              struct cli_client_priv *priv
                                  )
{
    int opts;

    while ((opts = getopt(argc, argv, CLI_CLI_CMDLINE_OPTS)) != -1) {
        switch (opts) {
            case 'n':
                cli_name = optarg;
            break;
        }
    }
}

void print_secs(struct command_subsections *ss, int sslen)
{
    int i;

    for (i = 0; i < sslen; i++) {
        printf("%s\n", ss[i].section);
    }
}

int cli_parse_by_space(
            struct command_subsections *ss,
            int subsections_maxlen,
            char *input,
            int len,
            struct cli_client_priv *priv
                      )
{
    int i = 0, j = 0;
    int ix = 0;

    for (i = 0, j = 0; i < len; i++) {
        if (input[i] == '\0' || input[i] == '\n') {
            ss[ix].section[j] = '\0';
            ix++;
            j = 0;
            goto newl;
        }
        if (input[i] == ' ') {
            ss[ix].section[j] = '\0';
            ix++;
            j = 0;
            continue;
        }
        if (ix > subsections_maxlen) {
            return -1;
        }
        ss[ix].section[j] = input[i];
        j++;
    }
    ss[ix].section[j] = '\0';
    ix++;

newl:
    return ix;
}

static struct clicmds {
    struct cli_commands *cmd;
} _cmds[] = {
    &show_cmd,
    &help_cmd,
};

void cli_get_cmdlist(void (*cbfunc)(struct cli_commands *))
{
    int i;

    for (i = 0; i < sizeof(_cmds) / sizeof(_cmds[0]); i ++) {
        cbfunc(_cmds[i].cmd);
    }
}

int cli_process_command(
                struct command_subsections *ss,
                int sslen,
                struct cli_client_priv *priv
                       )
{
    int len = sizeof(_cmds) / sizeof(_cmds[0]);
    int i;
    char *main_cmd = ss[0].section;

    if (!strcasecmp(main_cmd, "?")) {
        strcpy(main_cmd, "help");
    }

    for (i = 0; i < len; i++) {
        struct cli_commands *cmd = _cmds[i].cmd;

        if (!strcmp(cmd->command, main_cmd)) {
            cmd->cmdfunc(ss, sslen, cmd, priv);
        }
    }

    return 0;
}

int cli_parser(struct cli_client_priv *priv)
{
    while (1) {
        char input[1000];
        struct command_subsections sections[10];
        int sec_len = sizeof(sections) / sizeof(sections[0]);
        int parsed_len;

        memset(input, 0, sizeof(input));
        fprintf(stderr, "%s ", cli_name);
        fgets(input, sizeof(input), stdin);

        memset(sections, 0, sizeof(sections));

        parsed_len = cli_parse_by_space(
                                sections,
                                sec_len,
                                input,
                                strlen(input),
                                priv
                                       );
#ifdef CONFIG_CLI_DEBUG
        print_secs(sections, parsed_len);
#endif
        cli_process_command(sections, parsed_len, priv);
    }

    return -1;
}

int cli_client_initiate_server_conn(struct cli_client_priv *priv)
{
    int i;
    int retries = 10;

    // this is to allow CLI to retry connecting to the server
    // when the cli command is run by the inittab
    for (i = 0; i < retries; i ++) {
        priv->server_conn = libev_create_unix_tcp_conn(CLI_SRV_SOCK);

        if (priv->server_conn > 0)
            break;
        else
            fprintf(stderr, "failed to initiate CLI service connection : "
                                                "retry attempt [%d]\n", i);
    }

    if (priv->server_conn < 0) {
        fprintf(stderr, "failed to initiate CLI service connection\n");
        return -1;
    }

    return 0;
}

void cli_client_deinitiate_server_conn(struct cli_client_priv *priv)
{
    close(priv->server_conn);
}

int main(int argc, char **argv)
{
    int ret;
    struct cli_client_priv *priv;

    priv = calloc(1, sizeof(struct cli_client_priv));
    if (!priv)
        return -1;

    // register signals to block the cli term
    // signal(SIGINT, SIG_IGN);
    // signal(SIGQUIT, SIG_IGN);
    // signal(SIGPIPE, SIG_IGN);

    cli_client_cmdline_opts_parse(argc, argv, priv);

    ret = cli_client_initiate_server_conn(priv);
    if (ret)
        return -1;

    cli_parser(priv);

    return -1;
}

