#ifndef __SAPI_EVENT_H__
#define __SAPI_EVENT_H__

void *sapi_event_system_init(void);

void sapi_event_system_deinit(void *libctx);

void *sapi_read_evthreader(void *priv);

int sapi_reg_read_event(int sock, void *libctx, void *appctx,
                     void (*sapi_read_event_cb)(void *appctx));

void sapi_unreg_read_event(void *libctx, int sock);

void sapi_event_loop(void *libctx);

#endif
