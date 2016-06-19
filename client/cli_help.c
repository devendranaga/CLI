#include "cliincludes.h"
#include "cliintf.h"
#include "clicmd.h"
#include "clipriv.h"
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

void cli_service_help(
                     struct command_subsections *ss,
                     int args_len,
                     struct cli_commands *cli,
                     void *priv)
{
    int i, j;

    printf("args len %d\n", args_len);

    for (i = 0; i < args_len; i ++) {
        printf("-> %s\n", ss[i].section);
    }
}

