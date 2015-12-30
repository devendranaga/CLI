#ifndef __LIBNET_H__
#define __LIBNET_H__

struct libnet_if {
    char ifname[255];
    struct libnet_if *next;
};

#endif

int libnet_get_all_ifs(void *priv, struct libnet_if **list);
void libnet_free_all_ifs(void *priv, struct libnet_if **list);

