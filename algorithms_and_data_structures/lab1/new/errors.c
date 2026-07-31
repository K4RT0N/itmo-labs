#include <stdio.h>

#include "errors.h"

#ifdef DEBUG
#include "logfile.h"
#endif

void usage_error_message(void) // (1)
{
#   ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The user used the program incorrectly.\n");
    }
#   endif
    printf("isort [[-s|--static]|[-d|--dynamic]] --filename|-f <filename>\n"); // (1)
}