#include "libev.h"
#include "libnet.h"
#include "cliintf.h"
#include "clisrv.h"

#define CLI_SERVICE_AUTH_FILE_LOC "etc/passwd_cli"

static int __cli_service_auth_parse_line(char *line,
                                         struct cli_service_priv *priv)
{
    int ret;
    char *ptr;
    int len = strlen(line);
    char username[100];
    char passwd[100];
    char admin[100];
    int counter = 0;
    int i = 0;

    struct cli_auth_mgr *auth_mgr = priv->auth_mgr;

    line[len - 1] = '\0';

    memset(username, 0, sizeof(username));
    memset(passwd, 0, sizeof(passwd));
    memset(admin, 0, sizeof(admin));

    while (line && (*line != '\0') && (*line != ',')) {
        username[i] = *line;
        line ++;
        i ++;
    }

    username[i] = '\0';

    if (line) {
        line ++;
    } else {
        // no passwd and no auth ??
        return;
    }

    i = 0;
    while (line && (*line != '\0') && (*line != ',')) {
        passwd[i] = *line;
        line ++;
        i ++;
    }

    passwd[i] = '\0';

    if (line) {
        line ++;
    } else {
        // no auth ??
        return;
    }

    i = 0;
    while (line && (*line != '\0') && (*line != ',')) {
        admin[i] = *line;
        line ++;
        i ++;
    }

    admin[i] = '\0';

    printf("username %s\n", username);
    printf("passwd %s\n", passwd);
    printf("admin %s\n", admin);

    struct cli_auth_db *db_node;

    db_node = calloc(1, sizeof(struct cli_auth_db));
    if (!db_node) {
        return;
    }

    db_node->usrname = strdup(username);
    db_node->passwd = strdup(passwd);

    if (!strcmp(admin, "1")) {
        db_node->is_admin = 1;
    } else {
        db_node->is_admin = 0;
    }

    if (!auth_mgr->db) {
        auth_mgr->db = db_node;
    } else {
        struct cli_auth_db *tmp = auth_mgr->db;

        while (tmp->next) {
            tmp = tmp->next;
        }

        tmp->next = db_node;
    }

    return 0;
}

static int __cli_service_auth_create_file(struct cli_service_priv *priv)
{
    FILE *fp;

    fp = fopen(CLI_SERVICE_AUTH_FILE_LOC, "w");
    if (!fp)
        return -1;

    fprintf(fp, "user,password,1\n");
    fflush(fp);
    fclose(fp);

    return 0;
}

int cli_service_auth_init(struct cli_service_priv *priv)
{
    FILE *fp;
    char line[1024];
    int ret;

    priv->auth_mgr = calloc(1, sizeof(struct cli_auth_mgr));
    if (!priv->auth_mgr)
        return -1;

f_open:
    fp = fopen(CLI_SERVICE_AUTH_FILE_LOC, "r");
    if (!fp) {
        // first time the service has run .. lets create an auth file
        // with the dummy user
        ret = __cli_service_auth_create_file(priv);
        if (ret == 0)
            goto f_open;
        else {
            fprintf(stderr, "could not open the auth file %s..."
                            " do you have write permissions ?\n",
                            CLI_SERVICE_AUTH_FILE_LOC);
            return -1;
        }
    }

    while (fgets(line, sizeof(line), fp)) {
        __cli_service_auth_parse_line(line, priv);
    }

    fclose(fp);
    return 0;
}

