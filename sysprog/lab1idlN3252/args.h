#ifndef ARSG_H
#define ARSG_H

#include <stdbool.h>

typedef struct args_t {
    bool correct_usage;

    bool version_flag;
    bool help_flag;

    char *catalogue;
    char *target;
} args_t;

void parse_args(int argc, char **argv, args_t *args);

bool is_correct(args_t *args);
bool is_version(args_t *args);
bool is_help(args_t *args);

char *get_catalogue(args_t *args);
char *get_target(args_t *args);

#endif
