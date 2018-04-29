#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int util_api_file_exist(char *filename)
{
    int ret;
    struct stat st;

    ret = stat(filename, &st);
    if (ret < 0) {
	return 0;
    }

    return 1;
}

