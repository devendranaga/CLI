#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pthread.h>

struct sapi_read_evthread {
    int sock;
    pthread_cond_t read_cond;
    pthread_mutex_t cond_mutex;
    void *app_cb;
    void (*sapi_read_event_cb) (void *);
    void *priv;
};

struct sapi_read_evlist {
    pthread_t t;
    struct sapi_read_evthread *thread_data;
    struct sapi_read_evlist *next;
};

struct sapi_event_data {
    fd_set allfd;
    int maxfd;
    struct sapi_read_evlist *head;
    struct sapi_read_evlist *tail;
};

void *sapi_event_system_init(void)
{
    struct sapi_event_data *evt;

    evt = calloc(1, sizeof(struct sapi_event_data));
    if (!evt)
        return NULL;

    FD_ZERO(&evt->allfd);
    evt->maxfd = 0;

    return evt;
}

void sapi_event_system_deinit(void *libctx)
{
    struct sapi_event_data *evt = libctx;

    free(evt);
}

#define SET_THREAD_EVPRIV(__evthread, __libpriv) (__evthread->priv = __libpriv)

int sapi_read_evlist_add(struct sapi_event_data *sapi_evdata, pthread_t tid,
                            struct sapi_read_evthread *thrdata)
{
    struct sapi_read_evlist *list;

    list = calloc(1, sizeof(*list));
    if (!list)
        return -1;

    list->t = tid;
    list->thread_data = thrdata;

    if (!sapi_evdata->head) {
        sapi_evdata->head = list;
        sapi_evdata->tail = list;
    } else {
        sapi_evdata->tail->next = list;
        sapi_evdata->tail = list;
    }

    return 0;
}

void *sapi_read_evthreader(void *priv)
{
    struct sapi_read_evthread *thr_ctx = priv;

    for (;;) {
        // sleep until we woken up by select
        pthread_mutex_lock(&thr_ctx->cond_mutex);
        pthread_cond_wait(&thr_ctx->read_cond, &thr_ctx->cond_mutex);
        // since this runs in thread ... select can run and execute in parallel
        thr_ctx->sapi_read_event_cb(&thr_ctx->app_cb);
        pthread_mutex_unlock(&thr_ctx->cond_mutex);
    };
}

int sapi_reg_read_event(int sock, void *libctx, void *appctx,
                     void (*sapi_read_event_cb)(void *appctx))
{
    struct sapi_event_data *sapi_evdata = libctx;
    pthread_t t;
    struct sapi_read_evthread *new_tctx;

    FD_SET(sock, &sapi_evdata->allfd);

    if (sock > sapi_evdata->maxfd)
        sapi_evdata->maxfd = sock;

    new_tctx = calloc(1, sizeof(*new_tctx));
    if (!new_tctx)
        return -1;

    // setup new data structure for every new file descriptor
    new_tctx->sock = sock;
    SET_THREAD_EVPRIV(new_tctx, sapi_evdata);
    new_tctx->app_cb = appctx;
    new_tctx->sapi_read_event_cb = sapi_read_event_cb;
    pthread_cond_init(&new_tctx->read_cond, 0);
    pthread_mutex_init(&new_tctx->cond_mutex, 0);

    if (pthread_create(&t, NULL, sapi_read_evthreader, new_tctx) < 0)
        goto err_create_thread;

    return 0;

err_create_thread:
    free(new_tctx);
    return -1;
}

void sapi_unreg_read_event(void *libctx, int sock)
{
    struct sapi_event_data *sapi_evdata = libctx;
    struct sapi_read_evlist *list;
}

void sapi_event_loop(void *libctx)
{
    int ret;
    struct sapi_event_data *sapi_evdata = libctx;

    // entering infinite select loop
    for (;;) {
        fd_set rdfds = sapi_evdata->allfd;
        ret = select(sapi_evdata->maxfd, &rdfds, NULL, NULL, NULL);
        if (ret > 0) {
            struct sapi_read_evlist *list;

            for (list = sapi_evdata->head; list; list = list->next) {
                struct sapi_read_evthread *thread = list->thread_data;

                pthread_mutex_lock(&thread->cond_mutex);
                if (FD_ISSET(thread->sock, &rdfds)) {
                    pthread_cond_signal(&thread->read_cond);
                }
                pthread_mutex_unlock(&thread->cond_mutex);
            }
        }
    }
}
