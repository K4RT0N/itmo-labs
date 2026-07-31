#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <stdio.h>

/*
 * Program-wide debugging logfile.
 *
 * The pointer is opened by main only when DEBUG is enabled. Other modules use
 * it only after testing it against NULL, so diagnostics never change the
 * program behaviour when opening the logfile fails.
 */
extern FILE *logfile;

#endif
