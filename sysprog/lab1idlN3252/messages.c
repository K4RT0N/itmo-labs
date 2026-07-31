#include <stdio.h>

#include "messages.h"

#define PROGRAM_NAME "lab1idlN3252"

void invalid_target_message(void)
{
    printf("Invalid target format. (0x[hh*])\n");
}

void usage_message(void)
{
    printf("Usage: %s [options] catalogue target\n", PROGRAM_NAME);
}

void version_message(void)
{
    printf("lab1idlN3252 1.0.0 2026-03-26\n");
}

void help_message(void)
{
    usage_message();
    printf("Options:\n"
           "  -v|--version      Display version information.\n"
           "  -h|--help         Display this information.\n");
}
