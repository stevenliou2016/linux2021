#include "mem_manage.h"
#include <stdbool.h>
#include <stdio.h>

bool mem_alloc_succ(void *p)
{
    if (p == NULL) {
        printf("malloc failed\n");
        return false;
    }
    return true;
}
