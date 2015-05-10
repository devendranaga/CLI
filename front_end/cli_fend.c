#include "cli_fend.h"

int te;

void terminate(int si)
{
    te = 1;
    exit(0);
}

char *read_input(void)
{
    int c;
    int count = 0;
    int invalid_char = 0;
    char buff[MAX_CLI_INPUT];

    memset(buff, 0, sizeof(buff));

    while ((c = fgetc(stdin)) != EOF) {
        switch (c) {
            case '\n':
            if (invalid_char)
                return NULL;

                buff[count] = '\0';
                return strdup(buff);
            case '\'' :
            case '"' :
            case '#' :
            case '@':
            case '!':
            case '^':
            case '%':
            case '$':
            case '&':
            case '*':
            case '(':
            case ')':
            case '-':
            case '_':
            case '=':
            case '+':
            case '`':
            case '~':
            case '>':
            case '<':
            case '.':
            case ',':
            case '/':
            case '?':
            case ':':
            case ';':
            case '[':
            case ']':
            case '{':
            case '}':
            case '|':
            case '\\':
                invalid_char++;
            break;
        }

        buff[count] = c;
        count++;
    }
}

struct cli_context {
    int cli_conn;
} *cli_context;

int cli_api_clear(void *);
int cli_api_help(void *);
int cli_api_exit(void *);
int cli_api_list(void *);

struct cli_commands {
    const char *cmd;
    int cmd_depth;
    int (*cmd_exec)(void *);
    struct cli_commands *sub_list;
} cli_cmd_list[] = {
    {"clear", 0, cli_api_clear, NULL},
    {"help", 0, cli_api_help, &cli_cmd_list[0]},
    {"list", 0, cli_api_list, NULL},
    {"exit", 0, cli_api_exit, NULL},
};

#define CLI_CMD_SIZE ((sizeof(cli_cmd_list)) / (sizeof(cli_cmd_list[0])))

int cli_api_help(void *ptr)
{
    int ret;
    int i;

    printf("available commands:\n");
    for (i = 0; i < CLI_CMD_SIZE; i++)
        printf("\t%s\n", cli_cmd_list[i].cmd);
}

int cli_api_clear(void *ptr)
{
    struct cli_commands *subcmd = ptr;
    char buf[MAX_CLI_LEN];
    char *scr;

    tgetent(buf, getenv("TERM"));
    scr = tgetstr("cl", NULL);
    fputs(scr, stdout);

    return 0;
}

int cli_api_exit(void *ptr)
{
    exit(0);
}

int cli_api_list(void *ptr)
{
    struct cli_commands *subcmd = ptr;
    int ret = -1;
    DIR *dir;
    struct dirent *entry;
    char *curdir = ".";

    dir = opendir(curdir);
    if (!dir)
        return ret;

    while (entry = readdir(dir)) {
        if ((entry->d_type == DT_DIR) ||
            (entry->d_type == DT_REG)) {
            if (entry->d_name[0] != '.')
                printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);

    ret = 0;

    return ret;
}

void cli_process(void)
{
    int i;
    char *inp = read_input();
    int cmd_unavailable = 1;

    if (!inp) {
        printf("Invalid Characters\n");
        return;
    }

    for (i = 0; i < CLI_CMD_SIZE; i++) {
        if (!strcmp(cli_cmd_list[i].cmd, inp)) {
            cmd_unavailable = 0;
            cli_cmd_list[i].cmd_exec((void *)cli_cmd_list[i].sub_list);
        }
    }

    if (cmd_unavailable)
        printf("command %s unavailable\n", inp);

    free(inp);
}

int main(int argc, char *argv[])
{
    int ret;

    printf("Welcome (CLI version: %f "
           "author: DevNaga email: "
           "devendra.aaru@gmail.com)!\n\n\n", CLI_FEND_VER);

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGSTOP, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    printf("SIGUSR1 is %d\n", SIGUSR1);
    signal(SIGUSR1, terminate);

    cli_context = calloc(1, sizeof(struct cli_context));
    if (!cli_context)
        return -1;

    while (!te) {
        printf("CLI > ");
        cli_process();
    }
}

