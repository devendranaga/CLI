#include "libev.h"

struct libev_socket_list {
    int sock;
    libev_sock_type_t socktype;
    void *app_arg;
    void (*accept_func)(int sock, void *args);
    void (*recv_func)(int sock, void *args);
    void (*send_func)(int sock, void *args);
#define LIBEV_SOCK_HAS_TIMEOUT(__timeout) { \
    (((__timeout.tv_sec) != 0) || ((__timeout.tv_usec) != 0)) \
}
    struct timeval timeout;
    struct libev_socket_list *next;
};

struct libev_socket_context {
    int max_fd;
    fd_set fds;
    sigset_t sigmask;
    int sfd;
    struct libev_socket_list *head, *tail;
};

struct libev_signal_context {
    int sig;
    void *app_arg;
    void (*signal_func)(int sig, void *args);
    struct libev_signal_context *next;
};

struct libev_timer_context {
    long sec;
    long usec;
    int timer_fd;
    void (*timer_func)(void *args);
    struct libev_timer_context *next;
};

struct libev_context {
    struct libev_socket_context *sock_context;
    struct libev_timer_context *timer_context;
    struct libev_signal_context *signal_context;
};

void* libev_system_init()
{
    struct libev_context *context;
    struct libev_socket_context *sock_context;

    context = calloc(1, sizeof(struct libev_context));
    if (!context)
        return NULL;

    sock_context = calloc(1, sizeof(struct libev_socket_context));
    if (!sock_context)
        goto err;

    context->sock_context = sock_context;

    sigemptyset(&sock_context->sigmask);

    FD_ZERO(&sock_context->fds);

    // register termination handlers
    sigaddset(&sock_context->sigmask, SIGINT);
    sigaddset(&sock_context->sigmask, SIGQUIT);
    sigaddset(&sock_context->sigmask, SIGTERM);

    if (sigprocmask(SIG_BLOCK, &sock_context->sigmask, NULL))
        goto err;

    sock_context->sfd = signalfd(-1, &sock_context->sigmask, 0);
    if (sock_context->sfd < 0)
        goto err;

    FD_SET(sock_context->sfd, &sock_context->fds);

    return context;

err:
    if (sock_context)
        free(sock_context);
    if (context)
        free(context);
    return NULL;
}

void libev_system_deinit(void *ctx)
{
    free(ctx);
}

int _libev_register_sock(
                    int sock,
                    void *ctx,
                    void *app_arg,
                    void (*cbfunc)(int sock, void *app_arg),
                    libev_sock_type_t socktype
                        )
{
    struct libev_context *context = ctx;
    struct libev_socket_list *sock_node;
    struct libev_socket_context *sock_context;

    sock_context = context->sock_context;

    if (sock > sock_context->max_fd)
        sock_context->max_fd = sock;

    FD_SET(sock, &sock_context->fds);

    sock_node = calloc(1, sizeof(struct libev_socket_list));
    if (!sock_node)
        return -1;

    if (!sock_context->head) {
        sock_context->head = sock_node;
        sock_context->tail = sock_node;
    } else {
        sock_context->tail->next = sock_node;
        sock_context->tail = sock_node;
    }

    sock_node->app_arg = app_arg;
    switch (socktype) {
        case LIBEV_SOCK_TYPE_TCP_UNIX:
            sock_node->accept_func = cbfunc;
        break;
        case LIBEV_SOCK_TYPE_TCP_RECV:
            sock_node->recv_func = cbfunc;
        break;
        case LIBEV_SOCK_TYPE_UDP:
        case LIBEV_SOCK_TYPE_UDP_UNIX:
        case LIBEV_SOCK_TYPE_TCP: {
            free(sock_node);
            return -1;
        }
    }
    sock_node->sock = sock;
    sock_node->socktype = socktype;

    return 0;
}

int libev_register_tcp_unix_sock(
                            int sock,
                            void *ctx,
                            void *app_arg,
                            void (*accept_func)(int sock, void *app_arg)
                                )
{
    return _libev_register_sock(sock, ctx,
                                app_arg, accept_func,
                                LIBEV_SOCK_TYPE_TCP_UNIX);
}

int libev_register_sock(
                    int sock,
                    void *ctx,
                    void *app_arg,
                    void (*recv_func)(int sock, void *app_arg)
                       )
{
    return _libev_register_sock(sock, ctx,
                                app_arg, recv_func,
                                LIBEV_SOCK_TYPE_TCP_RECV);
}

static struct libev_socket_list *
libev_get_socket_node_by_sock(int sock, struct libev_context *context)
{
    struct libev_socket_context *sock_context = context->sock_context;
    struct libev_socket_list *sock_node;

    for (sock_node = sock_context->head;
            sock_node;
            sock_node = sock_node->next) {

        if (sock_node->sock == sock)
            return sock_node;
    }

    return NULL;
}

static void _libev_find_max_fd(struct libev_context *context)
{
    struct libev_socket_context *sock_context = context->sock_context;
    struct libev_socket_list *sock_node;
    int max_fd = 0;

    for (sock_node = sock_context->head;
            sock_node;
            sock_node = sock_node->next) {
        if (sock_node->sock > max_fd)
            max_fd = sock_node->sock;
    }

    context->sock_context->max_fd = max_fd;
}

static void _libev_free_sock_node(struct libev_context *context,
                                  struct libev_socket_list *node)
{
    struct libev_socket_context *sock_context = context->sock_context;
    struct libev_socket_list *sock_node = sock_context->head;
    struct libev_socket_list *sock_node_prev;

    if (sock_node == node) {
        sock_context->head = sock_node->next;
        free(node);
        return;
    }

    sock_node_prev = sock_node;

    while (sock_node) {
        if (sock_node == node) {
            sock_node_prev->next = sock_node->next;
            free(node);
            return;
        }

        sock_node_prev = sock_node;
        sock_node = sock_node->next;
    }
}

void _libev_unregister_sock(int sock, void *app_arg)
{
    struct libev_context *context = app_arg;
    struct libev_socket_list *node;

    node = libev_get_socket_node_by_sock(sock, context);
    if (!node)
        return;

    printf("unregister %d\n", sock);
    FD_CLR(node->sock, &context->sock_context->fds);
    _libev_free_sock_node(context, node);
    _libev_find_max_fd(context);
}

void libev_unregister_sock(int sock, void *app_arg)
{
    _libev_unregister_sock(sock, app_arg);
}

void libev_unregister_tcp_unix_sock(int sock, void *ctx)
{
    struct libev_context *context = ctx;
    struct libev_socket_list *node;

    node = libev_get_socket_node_by_sock(sock, context);
    if (!node)
        return;

    FD_CLR(node->sock, &context->sock_context->fds);
}

void libev_accept_func(struct libev_context *context,
                       struct libev_socket_list *sock_node)
{
    sock_node->accept_func(sock_node->sock, sock_node->app_arg);
}

void libev_recv_func(struct libev_context *context,
                     struct libev_socket_list *sock_node)
{
    sock_node->recv_func(sock_node->sock, sock_node->app_arg);
}

void libev_sock_event_func(fd_set *allfd, struct libev_context *context)
{
    struct libev_socket_context *sock_context = context->sock_context;
    struct libev_socket_list *sock_node;

    for (sock_node = sock_context->head; sock_node; sock_node = sock_node->next) {
        if (FD_ISSET(sock_node->sock, allfd)) {
            switch (sock_node->socktype) {
                case LIBEV_SOCK_TYPE_TCP_UNIX:
                    libev_accept_func(context, sock_node);
                break;
                case LIBEV_SOCK_TYPE_TCP_RECV:
                    libev_recv_func(context, sock_node);
                break;
                case LIBEV_SOCK_TYPE_TCP:
                case LIBEV_SOCK_TYPE_UDP:
                case LIBEV_SOCK_TYPE_UDP_UNIX:
                return;
            }
            FD_SET(sock_node->sock, &sock_context->fds);
        }
    }
}

int libev_signal_func(struct libev_context *context)
{
    struct signalfd_siginfo siginfo;
    struct libev_socket_context *sock_context = context->sock_context;
    int ret;
    int term_signal = 0;

    ret = read(sock_context->sfd, &siginfo, sizeof(siginfo));
    if (ret == sizeof(siginfo))  {
        if (siginfo.ssi_signo == SIGINT) {
            term_signal = 1;
        } else if (siginfo.ssi_signo == SIGQUIT) {
            term_signal = 1;
        } else if (siginfo.ssi_signo == SIGTERM) {
            term_signal = 1;
        }
    } else {
    }

    return term_signal;
}

void libev_main(void *ctx)
{
    struct libev_context *context = ctx;
    fd_set allfd;
    int ret;

    for (;;) {
        allfd = context->sock_context->fds;

        ret = select(context->sock_context->max_fd + 1,
                      &allfd, NULL, NULL, NULL);
        printf("Ret %d\n", ret);
        if (ret > 0) {
            if (FD_ISSET(context->sock_context->sfd, &allfd)) {
                ret = libev_signal_func(context);
                if (ret) {
                    fprintf(stderr, "Term signal received\n");
                    return;
                }
            } else {
                libev_sock_event_func(&allfd, context);
            }
        } else if (ret == 0) {
        } else if (ret < 0) {
            fprintf(stderr, "Exception occured %s\n", strerror(errno));
            return;
        }
    }
}

int libev_unix_tcp_init(void *ctx, char *path, void (*accept_func)(int sock, void *args), void *app_arg)
{
    int sock;
    struct sockaddr_un srv;
    struct libev_context *context = ctx;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;

    srv.sun_family = AF_UNIX;
    strcpy(srv.sun_path, path);

    unlink(path);

    if (bind(sock, (struct sockaddr *)&srv, strlen(srv.sun_path) + sizeof(srv.sun_family)) < 0)
        goto err;

    listen(sock, 4);

    libev_register_tcp_unix_sock(sock, context, app_arg, accept_func);
    return sock;

err:
    close(sock);
    return -1;
}

int libev_unix_tcp_conn(void *ctx, char *path, void (*recv_func)(int sock, void *args), void *app_arg)
{
    int sock;
    struct sockaddr_un srv;
    struct libev_context *context = ctx;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;

    srv.sun_family = AF_UNIX;
    strcpy(srv.sun_path, path);

    if (connect(sock, (struct sockaddr *)&srv, sizeof(srv) < 0))
        goto err;

    libev_register_sock(sock, context, app_arg, recv_func);
    return sock;

err:
    close(sock);
    return -1;
}

int libev_create_unix_tcp_conn(char *path)
{
    int sock;
    struct sockaddr_un srv;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;

    srv.sun_family = AF_UNIX;
    strcpy(srv.sun_path, path);

    if (connect(sock,
                (struct sockaddr *)&srv,
                strlen(srv.sun_path) +
                sizeof(srv.sun_family)) < 0) {
        perror("connect");
        goto err;
    }

    return sock;

err:
    close(sock);
    return -1;
}

void libev_unix_tcp_deinit(void *ctx, int sock)
{
    close(sock);
    libev_unregister_tcp_unix_sock(sock, ctx);
}

int _libev_signal_register(int sig, struct libev_context *context)
{
    struct libev_socket_context *sock_context;

    sock_context = context->sock_context;

    sigaddset(&sock_context->sigmask, sig);

    if (sigprocmask(SIG_UNBLOCK, &sock_context->sigmask, NULL) == -1)
        return -1;

    if (sigprocmask(SIG_BLOCK, &sock_context->sigmask, NULL) == -1)
        return -1;

    return 0;
}

int libev_signal_register(int sig, struct libev_context *context)
{
    return _libev_signal_register(sig, context);
}

int libev_term_signal_register(struct libev_context *context)
{
    int ret;

    ret = _libev_signal_register(SIGINT, context);
    if (ret)
        return ret;

    ret = _libev_signal_register(SIGQUIT, context);
    if (ret)
        return ret;

    return 0;
}

