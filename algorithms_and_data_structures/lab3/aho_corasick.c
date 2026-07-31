#include "aho_corasick.h"

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Every possible byte value is an edge label.  Using unsigned characters
 * avoids negative array indexes for bytes whose high bit is set.
 */
#define ALPHABET_SIZE 256

#ifdef DEBUG
#include "logfile.h"
#endif

/*
 * A trie node stores direct transitions, its Aho-Corasick failure transition
 * and an output chain for shorter patterns ending at the same text position.
 */
typedef struct ac_node {
    struct ac_node *children[ALPHABET_SIZE];
    struct ac_node *fail;
    int pattern_index;
    struct ac_node *output;
} ac_node_t;

/*
 * Complete automaton state.  patterns is borrowed from argv through args_t;
 * only root and its descendant nodes are owned by this structure.
 */
typedef struct {
    ac_node_t *root;
    int pattern_count;
    char **patterns;
} aho_corasick_t;

/*
 * Search results grow as matches are found.  This removes the previous
 * dict_size-sized limit, which was insufficient when one pattern occurred
 * many times in the same line.
 */
typedef struct {
    int *items;
    size_t length;
    size_t capacity;
} match_list_t;

static bool ac_search(aho_corasick_t *ac, char *text,
        match_list_t *matches);
static bool ac_build_trie(aho_corasick_t *ac, char **dictionary,
        int dict_size);
static aho_corasick_t* ac_init(char **dictionary, int dict_size);
static bool ac_build_failure_links(aho_corasick_t *ac);
static ac_node_t* ac_create_node(void);
static void ac_destroy_node(ac_node_t *node);
static bool ac_append_match(match_list_t *matches, int pattern_index);

/*
 * Builds an automaton, searches one text string and returns all dictionary
 * indices reported by the traversal.  All temporary trie resources are freed
 * before the function returns; ownership of the result array transfers to the
 * caller on success.
 */
int* aho_corasick(char *string, char **dictionary, int dict_size,
        int *result_size)
{
    aho_corasick_t *ac;
    match_list_t matches;

    if (result_size == NULL) {
        /* No valid output location exists for reporting the result length. */
        return NULL;
    }

    *result_size = 0;
    matches.items = NULL;
    matches.length = 0;
    matches.capacity = 0;

    if (string == NULL || dictionary == NULL || dict_size <= 0) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick received invalid search data.\n");
        }
#endif
        return NULL;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick automaton initialisation started. "
                "Patterns: %d.\n", dict_size);
    }
#endif

    ac = ac_init(dictionary, dict_size);
    if (ac == NULL) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick initialisation failed.\n");
        }
#endif
        return NULL;
    }

    /*
     * Trie construction and failure-link construction may allocate memory.
     * Either failure invalidates the incomplete automaton, so the common
     * cleanup path destroys all nodes before returning.
     */
    if (!ac_build_trie(ac, dictionary, dict_size)
            || !ac_build_failure_links(ac)) {
        ac_destroy_node(ac->root);
        free(ac);
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick automaton construction failed.\n");
        }
#endif
        return NULL;
    }

    /* The dynamic result list stores every occurrence, including repeats. */
    if (!ac_search(ac, string, &matches)) {
        free(matches.items);
        ac_destroy_node(ac->root);
        free(ac);
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick search stopped because result "
                    "storage could not be enlarged.\n");
        }
#endif
        return NULL;
    }

    /*
     * The public interface uses int result_size, so do not truncate an
     * unrealistically large match count into an invalid result.
     */
    if (matches.length > INT_MAX) {
        free(matches.items);
        ac_destroy_node(ac->root);
        free(ac);
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick match count exceeds the public "
                    "result-size range.\n");
        }
#endif
        return NULL;
    }

    ac_destroy_node(ac->root);
    free(ac);

    *result_size = (int)matches.length;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick search ended. Matches: %d.\n",
                *result_size);
    }
#endif

    return matches.items;
}

/*
 * Performs the public file-level search pass.  Each line owns its result
 * array, which is freed before getline advances to the next line.
 */
void find_all_entries(FILE *f, char **dictionary, int dict_size)
{
    int result_size;
    int *matches;
    char *lineptr;
    size_t n;
    int line_number;
    int i;

    lineptr = NULL;
    n = 0;
    line_number = 1;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Dictionary search through the input file started. "
                "Patterns: %d.\n", dict_size);
    }
#endif

    while (getline(&lineptr, &n, f) != -1) {
        /*
         * aho_corasick returns NULL both for zero matches and for allocation
         * failure.  result_size is initialised by the function, so the loop
         * below safely emits nothing in either case.
         */
        result_size = 0;
        matches = aho_corasick(lineptr, dictionary, dict_size, &result_size);

        for (i = 0; i < result_size; i++) {
            /*
             * Keep the original output text and ordering intact.  The match
             * array contains dictionary indexes in scan order.
             */
            printf("Pattern '%s' found at line %d: %s",
                    dictionary[matches[i]], line_number, lineptr);
        }

        /*
         * A non-NULL result belongs to this line only.  Releasing it here
         * prevents the per-line result leak present in the original version.
         */
        free(matches);
        line_number++;
    }

    free(lineptr);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Dictionary search through the input file ended. "
                "Processed lines: %d.\n", line_number - 1);
    }
#endif
}

/*
 * Allocates one zero-initialised trie node.  calloc clears every child link,
 * while the explicit fields document the terminal and output defaults.
 */
static ac_node_t* ac_create_node(void)
{
    ac_node_t *node;

    node = calloc(1, sizeof(ac_node_t));
    if (node == NULL) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick node allocation failed.\n");
        }
#endif
        return NULL;
    }

    node->pattern_index = -1;
    node->output = NULL;
    return node;
}

/*
 * Recursively frees the owned trie subtree.  Failure and output links are
 * non-owning cross-links, so only children are followed during destruction.
 */
static void ac_destroy_node(ac_node_t *node)
{
    int i;

    if (node == NULL) {
        /* An absent child is already fully destroyed. */
        return;
    }

    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            ac_destroy_node(node->children[i]);
        }
    }

    free(node);
}

/* Allocates and fills the automaton wrapper around a newly created root. */
static aho_corasick_t* ac_init(char **dictionary, int dict_size)
{
    aho_corasick_t *ac;

    ac = malloc(sizeof(aho_corasick_t));
    if (ac == NULL) {
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Aho-Corasick structure allocation failed.\n");
        }
#endif
        return NULL;
    }

    ac->root = ac_create_node();
    if (ac->root == NULL) {
        free(ac);
        return NULL;
    }

    ac->pattern_count = dict_size;
    ac->patterns = dictionary;
    return ac;
}

/*
 * Inserts each dictionary pattern into the trie.  A node is created only for
 * an absent transition; existing common prefixes are shared by patterns.
 */
static bool ac_build_trie(aho_corasick_t *ac, char **dictionary,
        int dict_size)
{
    ac_node_t *current;
    ac_node_t *new_node;
    char *pattern;
    int i;
    int j;
    int len;
    unsigned char idx;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick trie construction started.\n");
    }
#endif

    for (i = 0; i < dict_size; i++) {
        current = ac->root;
        pattern = dictionary[i];
        len = (int)strlen(pattern);

        for (j = 0; j < len; j++) {
            idx = (unsigned char)pattern[j];

            if (current->children[idx] == NULL) {
                /*
                 * This character has no outgoing edge from the current
                 * prefix, so extend the trie with a new zeroed node.
                 */
                new_node = ac_create_node();
                if (new_node == NULL) {
                    return false;
                }
                current->children[idx] = new_node;
            }

            current = current->children[idx];
        }

        /*
         * The terminal node records the matching dictionary index.  This
         * retains the original behaviour for duplicate dictionary words: the
         * later duplicate replaces the earlier terminal index.
         */
        current->pattern_index = i;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick trie construction ended.\n");
    }
#endif

    return true;
}

/*
 * Appends a node pointer to a dynamically resized breadth-first queue used
 * only while failure links are being built.
 */
static bool ac_enqueue(ac_node_t ***queue, size_t *capacity, size_t *rear,
        ac_node_t *node)
{
    ac_node_t **resized_queue;
    size_t new_capacity;

    if (*rear == *capacity) {
        /*
         * Grow geometrically to avoid a realloc for every node.  The queue
         * never removes from its allocated array; front simply advances.
         */
        new_capacity = *capacity == 0 ? 16 : *capacity * 2;
        if (new_capacity < *capacity
                || new_capacity > SIZE_MAX / sizeof(ac_node_t *)) {
            /* Size overflow would make an unsafe allocation request. */
            return false;
        }

        resized_queue = realloc(*queue, new_capacity * sizeof(ac_node_t *));
        if (resized_queue == NULL) {
            return false;
        }

        *queue = resized_queue;
        *capacity = new_capacity;
    }

    (*queue)[*rear] = node;
    (*rear)++;
    return true;
}

/*
 * Builds failure and output links by breadth-first traversal of the trie.
 * A dynamically resized queue replaces the former fixed 10,000-node buffer,
 * so large dictionaries no longer overwrite stack memory.
 */
static bool ac_build_failure_links(aho_corasick_t *ac)
{
    ac_node_t **queue;
    ac_node_t *current;
    ac_node_t *child;
    ac_node_t *fail;
    size_t front;
    size_t rear;
    size_t capacity;
    int i;
    unsigned char idx;

    queue = NULL;
    front = 0;
    rear = 0;
    capacity = 0;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick failure-link construction started.\n");
    }
#endif

    /*
     * Every first-level transition falls back to root.  These nodes seed the
     * BFS queue used to derive failure links for deeper nodes.
     */
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (ac->root->children[i] != NULL) {
            ac->root->children[i]->fail = ac->root;
            if (!ac_enqueue(&queue, &capacity, &rear,
                    ac->root->children[i])) {
                free(queue);
                return false;
            }
        }
    }

    while (front < rear) {
        current = queue[front];
        front++;

        for (i = 0; i < ALPHABET_SIZE; i++) {
            child = current->children[i];
            if (child == NULL) {
                /* No trie transition with this byte leaves current unchanged. */
                continue;
            }

            idx = (unsigned char)i;
            fail = current->fail;

            /*
             * Follow failure links until an existing transition with idx is
             * found or root is reached.
             */
            while (fail != ac->root && fail->children[idx] == NULL) {
                fail = fail->fail;
            }

            if (fail->children[idx] != NULL && fail->children[idx] != child) {
                child->fail = fail->children[idx];
            } else {
                /*
                 * Root has no suitable transition, so root itself is the
                 * fallback state for this child.
                 */
                child->fail = ac->root;
            }

            /*
             * If the failure target ends a pattern, it is the first output
             * link.  Otherwise inherit its next output link, preserving the
             * chain of suffix patterns.
             */
            if (child->fail->pattern_index != -1) {
                child->output = child->fail;
            } else {
                child->output = child->fail->output;
            }

            if (!ac_enqueue(&queue, &capacity, &rear, child)) {
                free(queue);
                return false;
            }
        }
    }

    free(queue);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Aho-Corasick failure-link construction ended. "
                "Nodes visited: %zu.\n", rear);
    }
#endif

    return true;
}

/*
 * Enlarges the match-result array when necessary and stores one dictionary
 * index.  The list is deliberately independent of dictionary size because a
 * single pattern may match multiple text positions.
 */
static bool ac_append_match(match_list_t *matches, int pattern_index)
{
    int *resized_items;
    size_t new_capacity;

    if (matches->length == matches->capacity) {
        new_capacity = matches->capacity == 0 ? 16 : matches->capacity * 2;
        if (new_capacity < matches->capacity
                || new_capacity > SIZE_MAX / sizeof(int)) {
            return false;
        }

        resized_items = realloc(matches->items, new_capacity * sizeof(int));
        if (resized_items == NULL) {
            return false;
        }

        matches->items = resized_items;
        matches->capacity = new_capacity;
    }

    matches->items[matches->length] = pattern_index;
    matches->length++;
    return true;
}

/*
 * Traverses text left to right.  At each state, failure links supply the
 * longest valid suffix transition when the next direct child is absent.
 */
static bool ac_search(aho_corasick_t *ac, char *text,
        match_list_t *matches)
{
    ac_node_t *current;
    ac_node_t *temp;
    int text_len;
    int i;
    unsigned char idx;

    current = ac->root;
    text_len = (int)strlen(text);

    for (i = 0; i < text_len; i++) {
        idx = (unsigned char)text[i];

        /*
         * If the current state lacks the input transition, repeatedly shorten
         * the matched prefix through failure links until a transition may be
         * used or the root state is reached.
         */
        while (current != ac->root && current->children[idx] == NULL) {
            current = current->fail;
        }

        if (current->children[idx] != NULL) {
            /* Consume the byte through the trie transition that exists. */
            current = current->children[idx];
        }

        /*
         * current records the longest match ending at i.  The output chain
         * then reports all shorter dictionary patterns that end at the same
         * character, in the original traversal order.
         */
        temp = current;
        while (temp != NULL) {
            if (temp->pattern_index != -1) {
                if (!ac_append_match(matches, temp->pattern_index)) {
                    return false;
                }
#ifdef DEBUG
                if (logfile) {
                    fprintf(logfile, "Pattern \"%s\" matched at character "
                            "%d.\n", ac->patterns[temp->pattern_index], i);
                }
#endif
            }
            temp = temp->output;
        }
    }

    return true;
}
