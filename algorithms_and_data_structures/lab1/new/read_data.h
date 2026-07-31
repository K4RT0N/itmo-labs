#ifndef _READ_DATA_H
#define _READ_DATA_H

#include <stdbool.h>
#include <stdio.h>

#include "array.h"

/*
 * Reads data from the specified file and stores it in an array
 */
bool read_data(FILE *f, array_type_t array_type,
               double **array_ptr, int *length);

#endif