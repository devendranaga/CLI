#include <stdio.h>
#include "libs/netlib/libnet.h"

int main()
{
    int ret;
    struct libnet_if *list = NULL;

    ret = libnet_get_all_ifs(NULL, &list);
    if (ret < 0) {
	printf("failed to get all ifs @ %s %u\n", __func__, __LINE__);
    } else {
	struct libnet_if *i;

	for (i = list; i; i = i->next) {
            printf("ifname <%s>\n", i->ifname);
	}
    }

    return 0;
}

