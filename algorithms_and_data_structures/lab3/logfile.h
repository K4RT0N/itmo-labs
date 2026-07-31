#ifndef _LOGFILE_H
#define _LOGFILE_H

#include <stdio.h>

/*
 * Shared debug log stream.
 *
 * The variable is defined in logfile.c and is created by main.c only
 * in a build made with the DEBUG target.  Other modules only write to it
 * after checking that the file was opened successfully.
 */
extern FILE *logfile;

#endif
