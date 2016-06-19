#ifndef __CLICMD_H__
#define __CLICMD_H__

struct command_subsections {
    int section_len;
    char section[1000];
};

struct cli_commands {
    char *command;          // command name
    CliPriv_t priv;         // privilege level
    // help message callback for the command
    void (*helpmsg)(int tab_level,
                    struct cli_commands *cmd,
                    void *priv);

    // command callback for the command
    void (*cmdfunc)(struct command_subsections *cmds,
                    int args_len,
                    struct cli_commands *cmd,
                    void *priv);

    // sub comamnd list
    struct cli_commands *sub_cmd;
    int sub_cmd_len;
};

extern struct cli_commands show_cmd;
extern struct cli_commands help_cmd;

#endif
