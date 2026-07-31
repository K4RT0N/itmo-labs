#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"

bool debug(void)
{
    char *path;
    path = getenv("LAB1DEBUG");
    if (path == NULL) {
        return false;
    }
    return true;
}
