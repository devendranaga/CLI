#ifndef __CLICMD_H__
#define __CLICMD_H__

struct command_subsections {
	int section_len;
	char section[1000];
};

struct cli_commands {
	char *command;
	void (*helpmsg)(int tab_level,
					struct cli_commands *cmd,
					void *priv);
	void (*cmdfunc)(struct command_subsections *cmds,
					int args_len,
					struct cli_commands *cmd,
					void *priv);
	struct cli_command *sub_cmd;
	int sub_cmd_len;
};

extern struct cli_commands show_cmd;

#endif
