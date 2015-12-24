#ifndef __CLICMD_H__
#define __CLICMD_H__

struct cli_commands {
	char *command;
	void (*helpmsg)(int tab_level,
					struct cli_commands *cmd,
					void *priv);
	void (*cmdfunc)(char *args,
					int args_len,
					struct cli_commands *cmd,
					void *priv);
	struct cli_command *sub_cmd;
	int sub_cmd_len;
};

#endif
