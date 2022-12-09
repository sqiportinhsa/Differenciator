#ifndef REPLACER
#define REPLACER

#include "../Tree/tree.h"
#include "../common.h"

struct Transformation {
    Tree_node* orig = nullptr;
    Tree_node* diff = nullptr;
};

void   save_transf(Tree_node *orig, Tree_node *diff, Transformation **transf);

size_t calc_subtree_weight(Tree_node *head);

bool   compare_subtrees(const Tree_node *head1, const Tree_node *head2);

int    make_replacings(Tree_node *head, Tree_node *node);

void   replace_same_in_subtree(const Tree_node *source, Tree_node *subtree);

#endif
