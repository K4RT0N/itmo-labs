#include <stdio.h>

#include "logfile.h"

/*
 * Shared logfile descriptor used by all modules compiled with DEBUG enabled.
 *
 * main owns opening and closing this stream. The initial NULL value allows any
 * module to safely skip logging before the program has initialized the file.
 */
FILE *logfile = NULL;
