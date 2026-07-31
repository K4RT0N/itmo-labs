#ifndef _ARGS_H
#define _ARGS_H

#include <stdbool.h>

typedef enum {
    AND_LOPER, OR_LOPER, UNDEFINED_LOPER
} loper_t;

typedef struct {
    /* Invormation about usage:
     * whether it's valid */
    bool valid_usage;

    /* Information about catalogues */
    char *catalogue;
    char *plugdir;

    /* Information about v and h flags */
    bool v;
    bool h;

    /* Logical operation information */
    bool negative_loper;
    loper_t loper;
} args_t;

void get_plugdir(int argc, char **argv, args_t *args);

#endif
