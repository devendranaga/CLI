#ifndef __LIBNET_H__
#define __LIBNET_H__

struct libnet_if {
    char ifname[255];
    struct libnet_if *next;
};


int libnet_get_all_ifs(void *priv, struct libnet_if **list);
void libnet_free_all_ifs(void *priv, struct libnet_if **list);
int libnet_get_if_ipv4(void *priv, char *ipaddr, char *ifname);

#endif

