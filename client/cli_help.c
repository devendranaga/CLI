#include "cliincludes.h"
#include "cliintf.h"
#include "clicmd.h"
#include "clipriv.h"
#include "clicli.h"
#include "cli_help.h"

void cli_service_help(
                     struct command_subsections *ss,
                     int args_len,
                     struct cli_commands *cli,
                     void *priv);

struct cli_commands help_cmd = {
    "help",
    CLI_PRIV_NO_PRIV,
    NULL,
    cli_service_help,
    NULL,
    0
};

void cli_command_set_callback(struct cli_commands *cmd)
{
    printf("     [%s]\n", cmd->command);
}

void cli_service_help(
                     struct command_subsections *ss,
                     int args_len,
                     struct cli_commands *cli,
                     void *priv)
{
    int i;

    for (i = 0; i < args_len; i ++) {
        if ((!strcasecmp(ss[i].section, "help")) ||
                (!strcasecmp(ss[i].section, "?"))) {
            cli_get_cmdlist(cli_command_set_callback);
        }
    }
}

