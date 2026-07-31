#ifndef _READ_DATA_H
#define _READ_DATA_H

#include <stdio.h>

#include "cqueue.h"

/*
 * Reads newline-separated floating-point values from f and appends each
 * valid value to cqueue.  Lines that are not exactly one number followed by
 * a newline are skipped, matching the original input format.
 */
void read_data(FILE *f, cqueue_t *cqueue);

#endif
