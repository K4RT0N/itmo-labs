#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

typedef struct {
    bool valid_usage;
    char *filename;
    char **dictionary;
    int dictionary_length;
} args_t;

void init_args(int argc, char **argv, args_t *args);
void destroy_args(args_t *args);

#endif
