#include <stdio.h>

void cli_memalloc_err(const char *file, const char *func, int line)
{
    fprintf(stderr, "failed to alloc at %s %s %u\n",
                            file, func, line);
}

