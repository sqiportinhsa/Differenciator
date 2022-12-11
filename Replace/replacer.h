#ifndef REPLACER
#define REPLACER

#include "../Tree/tree.h"
#include "../common.h"

struct Transformations {
    Tree_node** orig = nullptr;
    Tree_node** diff = nullptr;

    int index = 0;
};

void save_transf(Tree_node *orig, Tree_node *diff, Transformations *transf) ;

void make_replacings(Tree *tree1, Tree *tree2);
void make_replacings(Tree *tree,  Transformations *transfs);

void free_transfs(Transformations *transfs, bool free_copies = false);

#endif
