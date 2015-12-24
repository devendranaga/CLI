#include "libev.h"
#include "clisrv.h"
#include "clicmd.h"

#define CLI_CLI_CMDLINE_OPTS "n:"

static char *cli_name = "cli >";

struct cli_client_priv {
	int server_conn;
	void *libev_priv;
};

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

struct command_subsections {
	int section_len;
	char section[1000];
};

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

int cli_process_command(
	            struct command_subsections *ss,
	            int sslen,
	            struct cli_client_priv *priv
	                   )
{

}

int cli_parser(struct cli_client_priv *priv)
{
	while (1) {
		char input[1000];
		struct command_subsections sections[10];
		int sec_len = sizeof(sections) / sizeof(sections[0]);
		int parsed_len;

		fprintf(stderr, "%s ", cli_name);
		fgets(input, sizeof(input), stdin);
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
	}

	return -1;
}

int cli_client_initiate_server_conn(struct cli_client_priv *priv)
{
	priv->server_conn = libev_create_unix_tcp_conn(CLI_SRV_SOCK);
	if (priv->server_conn < 0)
		return -1;
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

	cli_client_cmdline_opts_parse(argc, argv, priv);

	priv->libev_priv = libev_system_init();
	if (!priv->libev_priv)
		return -1;

	cli_parser(priv);

	return -1;
}
