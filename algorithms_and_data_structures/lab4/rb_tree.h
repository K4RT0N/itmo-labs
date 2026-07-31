#ifndef _RB_TREE_H
#define _RB_TREE_H

#include <stdbool.h>

/*
 * Red-black tree used to store substring search results.
 *
 * The comparison order is lexical pattern order, then end position and then
 * dictionary index. The additional fields make nodes unique even when the same
 * pattern occurs more than once in one input line.
 */

/*
 * One tree node representing one pattern occurrence.
 *
 * pattern is a borrowed pointer to a dictionary string. The tree allocates and
 * frees nodes but never allocates or frees pattern text itself.
 */
typedef struct rb_tree_node_t {
    char *pattern;
    int end_position;
    int pattern_length;
    int pattern_index;
    bool is_red;
    struct rb_tree_node_t *left;
    struct rb_tree_node_t *right;
    struct rb_tree_node_t *parent;
} rb_tree_node_t;

/* Tree descriptor containing the root of the current result set. */
typedef struct {
    rb_tree_node_t *root;
} rb_tree_t;

/* Callback type used by in-order traversal for every visited result node. */
typedef void (*rb_tree_visit_t)(rb_tree_node_t *node, void *data);

/* Initializes tree without result nodes. */
void init_rb_tree(rb_tree_t *tree);

/* Frees every allocated node and restores the empty-tree state. */
void destroy_rb_tree(rb_tree_t *tree);

/*
 * Creates and inserts one result node.
 *
 * The function does not copy pattern; its caller must keep the dictionary
 * string alive until tree is destroyed and traversed.
 */
void insert_rb_tree(rb_tree_t *tree, char *pattern, int end_position,
        int pattern_length, int pattern_index);

/* Visits nodes in their comparison order, which starts with alphabetic order. */
void traverse_rb_tree(rb_tree_t *tree, rb_tree_visit_t visit, void *data);

#endif
