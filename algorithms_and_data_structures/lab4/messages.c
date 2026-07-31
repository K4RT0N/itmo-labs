#include <stdio.h>

#include "messages.h"

#ifdef DEBUG
#include "logfile.h"
#endif

void usage_message(void)
{
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The user used the program incorrectly.\n");
    }
#endif
    printf("./main <-f filename> [dictionary entities...]\n");
}
