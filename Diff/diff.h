#ifndef DIFF_DIFF
#define DIFF_DIFF

#include "../Tree/tree.h"
#include "../Replace/replacer.h"

void differenciation(Expression *expr);

void diff_tree(Tree *source, Tree *dest);

Tree_node* diff_node(Tree_node *source, Transformations *transf);

#endif
