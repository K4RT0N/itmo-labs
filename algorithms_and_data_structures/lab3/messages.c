#include <stdio.h>

#include "messages.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * Prints the historical usage text to standard output.  Keeping user-facing
 * diagnostics in this dedicated module lets main handle only control flow.
 */
void usage_message(void)
{
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Usage message requested.\n");
    }
#endif

    printf("./main <-f filename> "
           "<-n cqueue length> [dictionary entities...]\n");
}
