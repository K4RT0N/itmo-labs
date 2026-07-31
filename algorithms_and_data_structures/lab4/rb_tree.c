#include <stdlib.h>
#include <string.h>

#include "rb_tree.h"

#ifdef DEBUG
#include "logfile.h"
#endif

/* Rotates one subtree left around node during red-black insertion repair. */
static void rotate_left(rb_tree_t *tree, rb_tree_node_t *node);

/* Rotates one subtree right around node during red-black insertion repair. */
static void rotate_right(rb_tree_t *tree, rb_tree_node_t *node);

/* Restores colour and shape invariants after ordinary binary-tree insertion. */
static void fix_insertion(rb_tree_t *tree, rb_tree_node_t *node);

/* Recursively frees nodes belonging to one subtree. */
static void destroy_nodes(rb_tree_node_t *node);

/* Recursively visits one subtree in comparison order. */
static void traverse_nodes(rb_tree_node_t *node, rb_tree_visit_t visit,
        void *data);

/* Compares two result nodes using the tree's complete key. */
static int compare_nodes(rb_tree_node_t *first, rb_tree_node_t *second);

/* Treats a NULL child as black, as required by red-black-tree rules. */
static bool is_red(rb_tree_node_t *node);

/*
 * Initializes new empty red-black tree.
 *
 * A NULL root denotes an empty tree; NULL leaves are interpreted as black by
 * balancing code instead of allocating separate sentinel nodes.
 */
void init_rb_tree(rb_tree_t *tree)
{
    tree->root = NULL;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "An empty red-black tree was initialized.\n");
    }
#endif
}

/*
 * Destroys given red-black tree.
 *
 * Nodes contain borrowed pattern pointers, so only node allocations are freed.
 */
void destroy_rb_tree(rb_tree_t *tree)
{
    /* Post-order recursion releases children before their parent node. */
    destroy_nodes(tree->root);

    /* The descriptor must not retain a dangling root pointer. */
    tree->root = NULL;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Red-black tree nodes were destroyed.\n");
    }
#endif
}

/*
 * Inserts new match into red-black tree.
 *
 * First, the node is placed exactly as in an ordinary binary search tree.
 * Then fix_insertion restores red-black properties without changing the
 * comparison order of the stored result nodes.
 */
void insert_rb_tree(rb_tree_t *tree, char *pattern, int end_position,
        int pattern_length, int pattern_index)
{
    rb_tree_node_t *current;
    rb_tree_node_t *parent;
    rb_tree_node_t *new_node;
    int comparison;

    /* Allocate a node only after all result information is available. */
    new_node = malloc(sizeof(rb_tree_node_t));
    if (new_node == NULL) {
        /* Allocation failure leaves the existing result tree unchanged. */
#ifdef DEBUG
        if (logfile) {
            fprintf(logfile, "Unable to allocate memory for a red-black tree node.\n");
        }
#endif
        return;
    }

    /* New nodes start red; balancing will recolour or rotate when necessary. */
    new_node->pattern = pattern;
    new_node->end_position = end_position;
    new_node->pattern_length = pattern_length;
    new_node->pattern_index = pattern_index;
    new_node->is_red = true;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = NULL;

    /* Search for the insertion place while keeping the previous node as parent. */
    parent = NULL;
    current = tree->root;
    while (current != NULL) {
        parent = current;
        comparison = compare_nodes(new_node, current);

        if (comparison < 0) {
            /* Smaller key belongs to the left subtree. */
            current = current->left;
        } else if (comparison > 0) {
            /* Larger key belongs to the right subtree. */
            current = current->right;
        } else {
            /* Identical key is already present; discard duplicate allocation. */
#ifdef DEBUG
            if (logfile) {
                fprintf(logfile, "Duplicate red-black tree result was ignored: %s.\n",
                        pattern);
            }
#endif
            free(new_node);
            return;
        }
    }

    /* Link the new node to the located parent, or make it the first root. */
    new_node->parent = parent;
    if (parent == NULL) {
        /* The empty tree receives its first node as root. */
        tree->root = new_node;
    } else if (compare_nodes(new_node, parent) < 0) {
        /* New key is less than parent key, so attach it on the left. */
        parent->left = new_node;
    } else {
        /* New key is greater than parent key, so attach it on the right. */
        parent->right = new_node;
    }

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Search result '%s' was inserted into red-black tree.\n",
                pattern);
    }
#endif

    /* A red inserted node may violate colouring rules and requires repair. */
    fix_insertion(tree, new_node);
}

/*
 * Traverses red-black tree in sorted order.
 *
 * An in-order walk of a binary search tree visits keys from smallest to
 * largest, which here means alphabetical pattern order first.
 */
void traverse_rb_tree(rb_tree_t *tree, rb_tree_visit_t visit, void *data)
{
#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Red-black tree traversal started.\n");
    }
#endif

    traverse_nodes(tree->root, visit, data);

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Red-black tree traversal finished.\n");
    }
#endif
}

/*
 * Rotates subtree to the left around node.
 *
 * node's right child replaces node at the subtree root. The former right
 * child's left subtree becomes node's right subtree; all moved parents are
 * updated to retain a valid doubly linked tree structure.
 */
static void rotate_left(rb_tree_t *tree, rb_tree_node_t *node)
{
    rb_tree_node_t *right_child;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Red-black tree left rotation was performed.\n");
    }
#endif

    /* A left rotation is called only when node has a right child. */
    right_child = node->right;

    /* Move the inner subtree from right_child to node's right side. */
    node->right = right_child->left;
    if (right_child->left != NULL) {
        /* The transferred subtree now has node as its parent. */
        right_child->left->parent = node;
    }

    /* Connect the promoted child to the former parent of node. */
    right_child->parent = node->parent;
    if (node->parent == NULL) {
        /* Rotating at the root changes the tree root pointer. */
        tree->root = right_child;
    } else if (node == node->parent->left) {
        /* Node was a left child, so its parent now points left to right_child. */
        node->parent->left = right_child;
    } else {
        /* Node was a right child, so its parent now points right to right_child. */
        node->parent->right = right_child;
    }

    /* Finish rotation by placing the old subtree root below the promoted child. */
    right_child->left = node;
    node->parent = right_child;
}

/*
 * Rotates subtree to the right around node.
 *
 * This is the mirror operation of rotate_left: node's left child is promoted
 * and its right subtree is transferred to node's left side.
 */
static void rotate_right(rb_tree_t *tree, rb_tree_node_t *node)
{
    rb_tree_node_t *left_child;

#ifdef DEBUG
    if (logfile) {
        fprintf(logfile, "Red-black tree right rotation was performed.\n");
    }
#endif

    /* A right rotation is called only when node has a left child. */
    left_child = node->left;

    /* Move the inner subtree from left_child to node's left side. */
    node->left = left_child->right;
    if (left_child->right != NULL) {
        /* The transferred subtree now has node as its parent. */
        left_child->right->parent = node;
    }

    /* Connect the promoted child to the former parent of node. */
    left_child->parent = node->parent;
    if (node->parent == NULL) {
        /* Rotating at the root changes the tree root pointer. */
        tree->root = left_child;
    } else if (node == node->parent->right) {
        /* Node was a right child, so its parent now points right to left_child. */
        node->parent->right = left_child;
    } else {
        /* Node was a left child, so its parent now points left to left_child. */
        node->parent->left = left_child;
    }

    /* Finish rotation by placing the old subtree root below the promoted child. */
    left_child->right = node;
    node->parent = left_child;
}

/*
 * Restores red-black tree properties after insertion.
 *
 * The loop runs only while the inserted node has a red parent, which is the
 * only possible colour violation after adding one red node. The two major
 * branches are mirror cases depending on whether parent is a left or right
 * child of grandparent.
 */
static void fix_insertion(rb_tree_t *tree, rb_tree_node_t *node)
{
    rb_tree_node_t *parent;
    rb_tree_node_t *grandparent;
    rb_tree_node_t *uncle;

    /* Stop when node is root or its parent is already black. */
    while (node != tree->root && is_red(node->parent)) {
        parent = node->parent;
        grandparent = parent->parent;

        if (parent == grandparent->left) {
            /* Parent is left child: use the standard left-side balancing cases. */
            uncle = grandparent->right;

            if (is_red(uncle)) {
                /*
                 * Red uncle: recolour both children black and move the possible
                 * red-red violation upward to the recoloured grandparent.
                 */
                parent->is_red = false;
                uncle->is_red = false;
                grandparent->is_red = true;
                node = grandparent;
            } else {
                /* Black uncle: rotations will resolve the local violation. */
                if (node == parent->right) {
                    /*
                     * Inner "triangle" case. Rotate parent left to transform it
                     * into the outer "line" case handled immediately below.
                     */
                    node = parent;
                    rotate_left(tree, node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }

                /* Outer line case: recolour then rotate grandparent right. */
                parent->is_red = false;
                grandparent->is_red = true;
                rotate_right(tree, grandparent);
            }
        } else {
            /* Parent is right child: use the mirror image of left-side cases. */
            uncle = grandparent->left;

            if (is_red(uncle)) {
                /*
                 * Red uncle: recolour both children black and move the possible
                 * red-red violation upward to the recoloured grandparent.
                 */
                parent->is_red = false;
                uncle->is_red = false;
                grandparent->is_red = true;
                node = grandparent;
            } else {
                /* Black uncle: rotations will resolve the local violation. */
                if (node == parent->left) {
                    /*
                     * Inner "triangle" case. Rotate parent right to transform it
                     * into the outer "line" case handled immediately below.
                     */
                    node = parent;
                    rotate_right(tree, node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }

                /* Outer line case: recolour then rotate grandparent left. */
                parent->is_red = false;
                grandparent->is_red = true;
                rotate_left(tree, grandparent);
            }
        }
    }

    /* The root is always black by the defining red-black-tree invariant. */
    tree->root->is_red = false;
}

/*
 * Frees all nodes in given subtree using post-order traversal.
 *
 * Children are released before their parent so recursive calls never follow a
 * pointer from already freed memory.
 */
static void destroy_nodes(rb_tree_node_t *node)
{
    if (node == NULL) {
        /* Empty child represents a black leaf and owns no allocation. */
        return;
    }

    /* Free complete left and right subtrees before freeing current node. */
    destroy_nodes(node->left);
    destroy_nodes(node->right);
    free(node);
}

/*
 * Visits all nodes in given subtree in sorted order.
 *
 * Left subtree contains smaller keys, current node is next, and right subtree
 * contains larger keys. This is why the output starts in alphabetical order.
 */
static void traverse_nodes(rb_tree_node_t *node, rb_tree_visit_t visit,
        void *data)
{
    if (node == NULL) {
        /* Empty child contributes no result to the traversal. */
        return;
    }

    /* Visit smaller keys, then current key, then larger keys. */
    traverse_nodes(node->left, visit, data);
    visit(node, data);
    traverse_nodes(node->right, visit, data);
}

/*
 * Compares two nodes using the complete tree key.
 *
 * Pattern text is primary key and creates alphabetical output. end_position
 * keeps repeated occurrences of one pattern distinct, while pattern_index is
 * the final tie breaker for otherwise identical results.
 */
static int compare_nodes(rb_tree_node_t *first, rb_tree_node_t *second)
{
    int comparison;

    /* Lexical comparison defines the requested alphabetical primary order. */
    comparison = strcmp(first->pattern, second->pattern);
    if (comparison != 0) {
        /* Distinct text already provides a strict comparison result. */
        return comparison;
    }

    if (first->end_position != second->end_position) {
        /* Equal patterns are ordered by their occurrence position in the line. */
        return first->end_position - second->end_position;
    }

    /* Same text at same position is finally ordered by original dictionary index. */
    return first->pattern_index - second->pattern_index;
}

/*
 * Tells if given node is red.
 *
 * NULL children represent black leaves in this implementation, therefore this
 * helper is safe for both allocated nodes and absent children.
 */
static bool is_red(rb_tree_node_t *node)
{
    return node != NULL && node->is_red;
}
