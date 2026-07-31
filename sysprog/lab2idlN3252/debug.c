#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include "debug.h"

void debug(const char *format, ...)
{
    va_list args;
    char *envval;

    envval = getenv("LAB2DEBUG");
    if (!envval) {
        return;
    }

    va_start(args, format);

    vprintf(format, args);

    va_end(args);
}
