#include <stdio.h>
#include "libs/eventlib/libev.h"
#include <sys/socket.h>
#include <netinet/in.h>

void accept_conns(int sock, void *args)
{
    int cli_conn;
    struct sockaddr_in client;
    socklen_t client_len = 0;

    cli_conn = accept(sock, (struct sockaddr *)&client, &client_len);
    if (cli_conn < 0) {
	return;
    }

    printf("client connection %d created\n", cli_conn);
    exit(0);
}

int main()
{
    int server_conn;
    int client_conn;
    int udp_conn_serv;
    void *handle;

    handle = libev_system_init();

    server_conn = libev_unix_tcp_init(handle, "/tmp/clisrv.sock", accept_conns, NULL);
    if (server_conn < 0) {
	printf("failed to create server tcp @ %s %u\n",
					__func__, __LINE__);
	return 0;
    }

    // test when server is offline / online
    client_conn = libev_create_unix_tcp_conn("/tmp/clisrv.sock");
    if (client_conn < 0) {
	printf("failed to create server connection @ %s %u\n",
					__func__, __LINE__);
	return 0;
    }

    udp_conn_serv = libev_unix_udp_init(handle, "/tmp/udpconn.sock");
    if (udp_conn_serv < 0) {
	printf("failed to create server udp @ %s %u\n",
					__func__, __LINE__);
	return 0;
    }

    printf("connection %d of udp\n", udp_conn_serv);
    libev_main(handle);

    return 0;
}

