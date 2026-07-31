#ifndef _AHO_CORASICK_H
#define _AHO_CORASICK_H

#include <stdio.h>

/*
 * Searches one string with the supplied dictionary.
 *
 * The returned dynamically allocated array contains dictionary indices in
 * the order in which the original Aho-Corasick traversal reports matches.
 * The caller owns this array and must free it.  result_size receives zero
 * when there are no matches or when the search cannot be completed.
 */
int* aho_corasick(char *string, char **dictionary, int dict_size,
        int *result_size);

/*
 * Reads f line by line, searches each line with dictionary and prints every
 * matching pattern using the program's historical output format.
 */
void find_all_entries(FILE *f, char **dictionary, int dict_size);

#endif
