#ifndef __LIBEV_H__
#define __LIBEV_H__

#include "cliincludes.h"

typedef enum {
    LIBEV_SOCK_TYPE_TCP,
    LIBEV_SOCK_TYPE_TCP_UNIX,
    LIBEV_SOCK_TYPE_TCP_RECV,
    LIBEV_SOCK_TYPE_UDP,
    LIBEV_SOCK_TYPE_UDP_UNIX,
} libev_sock_type_t;

void libev_main(void *ctx);

void libev_unregister_tcp_unix_sock(int sock, void *ctx);
void libev_unregister_sock(int sock, void *app_arg);

int libev_register_tcp_unix_sock(int sock,
                                 void *ctx,
                                 void *app_arg,
                                 void (*cbfunc)(int sock, void *args));

void libev_system_deinit(void *ctx);

void* libev_system_init();

int libev_unix_tcp_init(void *ctx,
                        char *path,
                        void (*accept_func)(int sock, void *args),
                        void *app_arg);

void libev_unix_tcp_deinit(void *ctx, int sock);
int libev_create_unix_tcp_conn(char *path);
int libev_register_sock(
                    int sock,
                    void *ctx,
                    void *app_arg,
                    void (*recv_func)(int sock, void *app_arg)
                       );

#endif

