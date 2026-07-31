#ifndef _BOYER_MOORE_H
#define _BOYER_MOORE_H

#include <stdio.h>

#include "rb_tree.h"

/*
 * Searches every dictionary entry in one string using the bad-character
 * version of Boyer-Moore algorithm.
 *
 * Each match is inserted into result_tree. The tree owns only its nodes; the
 * pattern pointers remain owned by the command-line dictionary.
 */
void boyer_moore(char *string, char **dictionary, int dict_size,
        rb_tree_t *result_tree);

/*
 * Reads the input file line by line, searches every line and prints matches.
 *
 * A separate red-black tree is created for each line so alphabetical ordering
 * is preserved inside a line without mixing matches from different lines.
 */
void find_all_entries(FILE *f, char **dictionary, int dict_size);

#endif
