#include <stdio.h>

#include "messages.h"

void usage_message(char *filename) 
{
#   ifdef DEBUG
    fprintf(stderr, "usage_message function started.\n");
#   endif
    printf("Usage: %s [--file filename] [--<static|dynamic>]\n", filename);
#   ifdef DEBUG
    fprintf(stderr, "usage_message function ended.\n");
#   endif
}
