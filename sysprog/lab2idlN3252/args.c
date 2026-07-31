#include <stdbool.h>
#include <string.h>

#include "args.h"

void get_plugdir(int argc, char **argv, args_t *args)
{
    bool valid_usage;
    char *plugdir;
    bool p_flag;
    int i;

    valid_usage = true;
    p_flag = false;
    plugdir = NULL;

    i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-P") == 0) {
            if (i == argc - 1 || p_flag) {
                valid_usage = false;
                plugdir = NULL;
                i = argc;
            } else {
                plugdir = argv[i + 1];
                p_flag = true;
                i += 2;
            }
        } else {
            i++;
        }
    }

    args->valid_usage = valid_usage;
    args->plugdir = plugdir;
}
