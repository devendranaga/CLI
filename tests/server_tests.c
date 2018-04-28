#include <stdio.h>
#include "libs/eventlib/libev.h"

int main()
{
    int server_conn;

    // test when server is offline / online
    server_conn = libev_create_unix_tcp_conn("/tmp/clisrv.sock");
    if (server_conn < 0) {
	printf("failed to create server connection @ %s %u\n",
					__func__, __LINE__);
    }

    return 0;
}

