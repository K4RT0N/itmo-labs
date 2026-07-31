#include <stdio.h>

#include "messages.h"

/* Prints both supported option spellings and leaves error handling to main. */
void usage_message(void)
{
    printf("Usage: ./riccati_solver -f problem.txt [-o solution.csv]\n");
    printf("       ./riccati_solver --file problem.txt [--output solution.csv]\n");
}
