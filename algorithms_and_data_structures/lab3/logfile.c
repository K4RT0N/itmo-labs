#include <stdio.h>

#include "logfile.h"

/*
 * A single log stream is shared by all modules in a DEBUG build.
 * It remains NULL until main.c successfully opens the "logfile" file.
 */
FILE *logfile = NULL;
