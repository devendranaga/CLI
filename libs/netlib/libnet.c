#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "cliincludes.h"
#include "libnet.h"

struct libnet_priv {
};

static int __libnet_is_ifname_present(char *ifname, struct libnet_if *head)
{
    struct libnet_if *entry;

    for (entry = head; entry; entry = entry->next) {
        if (!strcmp(entry->ifname, ifname))
            return 1;
    }

    return 0;
}

int libnet_get_all_ifs(void *priv, struct libnet_if **list)
{
    struct ifaddrs *iflist, *ifa;
    struct libnet_if *new_list = NULL, *next = NULL;

    if (getifaddrs(&iflist) == -1) {
        fprintf(stderr, "failed to getifaddrs %s\n", strerror(errno));
        return -1;
    }

    for (ifa = iflist; ifa; ifa = ifa->ifa_next) {
        struct libnet_if *lif = NULL;

        lif = calloc(1, sizeof(struct libnet_if));
        if (!lif)
            return -1;

        if (!__libnet_is_ifname_present(ifa->ifa_name, new_list)) {
            strcpy(lif->ifname, ifa->ifa_name);
            if (!new_list) {
                new_list = lif;
                next = lif;
            } else {
                next->next = lif;
                next = lif;
            }
        }
    }

    *list = new_list;
    freeifaddrs(iflist);

    return 0;
}

int libnet_get_if_ipv4(void *priv, char *ipaddr, char *ifname)
{
    int sk;
    char *ip;
    struct ifreq ifr;

    sk = socket(AF_INET, SOCK_DGRAM, 0);
    if (sk < 0)
        return -1;

    memset(&ifr, 0, sizeof(ifr));

    ifr.ifr_addr.sa_family = AF_INET;
    strcpy(ifr.ifr_name, ifname);

    if (ioctl(sk, SIOCGIFADDR, &ifr) < 0) {
        close(sk);
        return -1;
    }

    ip = inet_ntoa((((struct sockaddr_in *)&ifr.ifr_addr))->sin_addr);
    strcpy(ipaddr, ip);

    close(sk);
    return 0;
}

void libnet_free_all_ifs(void *priv, struct libnet_if **list)
{
    struct libnet_if *ifl, *ifl_prev;

    ifl = ifl_prev = *list;
    while (ifl) {
        ifl = ifl->next;
        free(ifl_prev);
        ifl_prev = ifl;
    }
}

