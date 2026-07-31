#include "boyer_moore.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#include "logfile.h"
#endif

/* Number of possible byte values used by the bad-character table. */
#define ALPHABET_SIZE 256

/*
 * Context passed to the tree traversal callback.
 *
 * The node stores only the index of a dictionary entry, therefore the callback
 * also needs the dictionary itself and information about the current line.
 */
typedef struct {
    char **dictionary;
    int line_number;
    char *lineptr;
} print_data_t;

/* Searches all occurrences of one non-empty pattern in one input string. */
static void find_pattern_entries(char *string, char *pattern,
        int pattern_index, rb_tree_t *result_tree);

/* Prints one match represented by a red-black tree node. */
static void print_entry(rb_tree_node_t *node, void *data);

/* Checks whether the same pattern occurs later in the dictionary. */
static bool is_duplicate(char **dictionary, int current_index, int dict_size);

/* Adds entries for the last empty dictionary pattern, if such pattern exists. */
static void save_empty_pattern_entries(char *string, char **dictionary,
        int dict_size, rb_tree_t *result_tree);

/*
 * Main Boyer-Moore function.
 *
 * Every distinct non-empty dictionary entity is searched separately. A result
 * tree receives all occurrences and later provides alphabetical output order.
 */
void boyer_moore(char *string, char **dictionary, int dict_size,
        rb_tree_t *result_tree)
{
    int i;

    if (string == NULL || dictionary == NULL || dict_size <= 0) {
        /* Invalid input cannot contain a searchable dictionary entry. */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Boyer-Moore received invalid search data.\n");
        }
#endif
        return;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Boyer-Moore started searching %d dictionary entries.\n",
                dict_size);
    }
#endif

    /* Inspect every dictionary entity in the original command-line order. */
    for (i = 0; i < dict_size; i++) {
        if (dictionary[i][0] == '\0') {
            /* Empty entries are handled separately because BM needs length > 0. */
            continue;
        }
        if (is_duplicate(dictionary, i, dict_size)) {
            /* Keep only the last identical entry, matching previous behaviour. */
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Duplicate dictionary entity was skipped: %s.\n",
                        dictionary[i]);
            }
#endif
            continue;
        }

        /* Search all occurrences of the current unique non-empty pattern. */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Boyer-Moore started searching pattern: %s.\n",
                    dictionary[i]);
        }
#endif
        find_pattern_entries(string, dictionary[i], i, result_tree);
    }

    /* Empty pattern has dedicated handling and the old output semantics. */
    save_empty_pattern_entries(string, dictionary, dict_size, result_tree);
}

/*
 * Finds dictionary entries in every line of an opened file.
 *
 * getline owns and, when necessary, enlarges lineptr. It is released once
 * after the last line. The result tree is intentionally scoped to one line.
 */
void find_all_entries(FILE *f, char **dictionary, int dict_size)
{
    rb_tree_t result_tree;
    print_data_t print_data;

    char *lineptr;
    size_t n;

    int line_number;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program started searching dictionary entries "
                "in file line-by-line.\n");
    }
#endif

    /* getline requires NULL on its first call when it should allocate buffer. */
    lineptr = NULL;
    line_number = 1;

    /* Read input one complete line at a time, including its trailing newline. */
    while (getline(&lineptr, &n, f) != -1) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "The program started searching line %d.\n",
                    line_number);
        }
#endif

        /* Start with an empty tree that will contain only current-line matches. */
        init_rb_tree(&result_tree);

        /* Insert every match of the current line into alphabetical tree. */
        boyer_moore(lineptr, dictionary, dict_size, &result_tree);

        /* Prepare data needed by the traversal callback. */
        print_data.dictionary = dictionary;
        print_data.line_number = line_number;
        print_data.lineptr = lineptr;

        /* In-order traversal prints matches in the tree comparison order. */
        traverse_rb_tree(&result_tree, print_entry, &print_data);

        /* Nodes are no longer needed after all matches for this line are printed. */
        destroy_rb_tree(&result_tree);

        /* The next successful getline call belongs to the following line. */
        line_number++;
    }

    /* Release the dynamic line buffer allocated and reused by getline. */
    free(lineptr);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "The program finished searching dictionary entries "
                "in file.\n");
    }
#endif
}

/*
 * Finds every occurrence of one non-empty pattern using Boyer-Moore search.
 *
 * Only the bad-character rule is used. Characters are compared from pattern
 * end to beginning; after a mismatch the alignment moves by the largest safe
 * positive shift. A successful match also advances far enough to find overlaps.
 */
static void find_pattern_entries(char *string, char *pattern,
        int pattern_index, rb_tree_t *result_tree)
{
    int bad_char[ALPHABET_SIZE];
    int string_length;
    int pattern_length;
    int i;
    int j;
    int shift;

    string_length = strlen(string);
    pattern_length = strlen(pattern);

    /* Default -1 means a byte does not occur in the pattern. */
    for (i = 0; i < ALPHABET_SIZE; i++) {
        bad_char[i] = -1;
    }

    /* Save the rightmost position of every byte in the pattern. */
    for (i = 0; i < pattern_length; i++) {
        bad_char[(unsigned char)pattern[i]] = i;
    }

    /* shift is the current index where pattern starts in string. */
    shift = 0;
    while (shift <= string_length - pattern_length) {
        /* Begin comparison at the pattern's rightmost character. */
        j = pattern_length - 1;

        /* Move left while the two aligned characters are equal. */
        while (j >= 0 && pattern[j] == string[shift + j]) {
            j--;
        }

        if (j < 0) {
            /* All characters matched, therefore save this occurrence in tree. */
            insert_rb_tree(result_tree, pattern, shift + pattern_length - 1,
                    pattern_length, pattern_index);
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Pattern '%s' was found at position %d.\n",
                        pattern, shift + 1);
            }
#endif

            if (shift + pattern_length < string_length) {
                /*
                 * A following byte exists. Its rightmost pattern occurrence
                 * determines the shift and still allows overlapping matches.
                 */
                shift += pattern_length - bad_char[(unsigned char)
                        string[shift + pattern_length]];
            } else {
                /* The match ended at string end, so a final one-step advance ends loop. */
                shift++;
            }
        } else {
            /*
             * A mismatch occurred at j. Align its text byte with its rightmost
             * occurrence in pattern; when absent or too far right, move once.
             */
            i = j - bad_char[(unsigned char)string[shift + j]];
            if (i > 0) {
                /* The bad-character rule permits a larger positive shift. */
                shift += i;
            } else {
                /* Never keep the same alignment after a mismatch. */
                shift++;
            }
        }
    }
}

/*
 * Prints one tree node in the original output format.
 *
 * The callback receives nodes in alphabetical order because traversal is
 * in-order. pattern_index maps a node back to the original dictionary entry.
 */
static void print_entry(rb_tree_node_t *node, void *data)
{
    print_data_t *print_data;

    /* Interpret generic callback data as the context prepared by caller. */
    print_data = data;

    /* Keep exact output text used by the original program. */
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Pattern '%s' was printed for line %d.\n",
                print_data->dictionary[node->pattern_index],
                print_data->line_number);
    }
#endif
    printf("Pattern '%s' found at line %d: %s",
            print_data->dictionary[node->pattern_index],
            print_data->line_number, print_data->lineptr);
}

/*
 * Checks if current dictionary entry occurs again later in the dictionary.
 *
 * Searching only the last equal entry prevents duplicate output while retaining
 * the same chosen dictionary index as the previous implementation.
 */
static bool is_duplicate(char **dictionary, int current_index, int dict_size)
{
    int i;

    /* Compare current entry with every following command-line entity. */
    for (i = current_index + 1; i < dict_size; i++) {
        if (strcmp(dictionary[current_index], dictionary[i]) == 0) {
            /* A later duplicate will be searched instead of the current one. */
            return true;
        }
    }

    /* No following entry has the same text. */
    return false;
}

/*
 * Saves entries for the last empty dictionary pattern.
 *
 * Boyer-Moore cannot process an empty pattern because it compares from index
 * pattern_length - 1. The explicit loop preserves the original treatment of
 * the last empty dictionary entity.
 */
static void save_empty_pattern_entries(char *string, char **dictionary,
        int dict_size, rb_tree_t *result_tree)
{
    int i;
    int empty_pattern_index;

    /* -1 means no empty dictionary entry has been observed yet. */
    empty_pattern_index = -1;

    /* Keep the last empty entry because duplicate handling uses last occurrence. */
    for (i = 0; i < dict_size; i++) {
        if (dictionary[i][0] == '\0') {
            /* Remember this index and allow a later empty entry to replace it. */
            empty_pattern_index = i;
        }
    }

    if (empty_pattern_index == -1) {
        /* There is no empty pattern, so no special entries must be inserted. */
        return;
    }

    /* Add one zero-length occurrence for each character position in string. */
    for (i = 0; string[i] != '\0'; i++) {
        insert_rb_tree(result_tree, dictionary[empty_pattern_index], i, 0,
                empty_pattern_index);
    }
}
