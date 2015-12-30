#ifndef __CLIINTF_H__
#define __CLIINTF_H__

#define CLI_COMMAND_RESP_LEN 1000

typedef enum {
    CLI_INTF_CMD_REQ,
    CLI_INTF_CMD_RESP,
} CliInterfaceType_t;

typedef enum {
    CLI_COMMAND_RES_SUCCESS,
    CLI_COMMAND_RES_FAIL,
} CliCommandRes_t;

typedef enum {
    CLI_PRIV_NO_PRIV,
    CLI_PRIV_SU_PRIV,
} CliPriv_t;

typedef enum {
    CLI_COMMAND_SHOW,
} CliCommands_t;

typedef enum {
    CLI_SUBCOMMAND_SHOW_DATE,
    CLI_SUBCOMMAND_SHOW_CLI_VER,
} CliSubCommands_t;

struct cli_interface_cmdreq {
    CliCommands_t command;
    CliSubCommands_t sub_command;
    CliPriv_t priv;
    uint16_t datalen;
    uint8_t data[0];
} __attribute__((__packed__));

struct cli_interface_cmdresp {
    CliCommands_t command;
    CliSubCommands_t sub_command;
    CliCommandRes_t res;
    uint16_t datalen;
    uint8_t data[0];
} __attribute__((__packed__));

struct cli_interface {
    CliInterfaceType_t type;
    uint16_t len;
    uint8_t data[0];
} __attribute__((__packed__));

#endif
