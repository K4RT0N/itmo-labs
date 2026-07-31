#ifndef READ_DATA_H
#define READ_DATA_H

#include <stdbool.h>

#include "codes.h"

/*
 * Reads data from file with name filename into static or dynamic array.
 */
code_t read_data(double **array, int *length, char *filename, bool dynamic);

#endif
