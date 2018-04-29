#include <stdio.h>
#include <string.h>
#include "libs/netlib/libnet.h"

int main()
{
    int ret;
    struct libnet_if *list = NULL;
    char ipaddr[200];

    ret = libnet_get_all_ifs(NULL, &list);
    if (ret < 0) {
	printf("failed to get all ifs @ %s %u\n", __func__, __LINE__);
    } else {
	struct libnet_if *i;

	for (i = list; i; i = i->next) {
	    memset(ipaddr, 0, sizeof(ipaddr));

	    // +ve
	    ret = libnet_get_if_ipv4(NULL, ipaddr, i->ifname);
	    if (ret) {
		printf("failed to get v4 addr for ifname %s @ %s %u\n",
						i->ifname, __func__, __LINE__);
		continue;
	    }

	    printf("ifname <%s> ipv4 <%s>\n",
			    	i->ifname, ipaddr);
	}
    }

    // -ve
    libnet_get_if_ipv4(NULL, ipaddr, "dev0");

    libnet_free_all_ifs(NULL, &list);

    return 0;
}

