#include "../Tree/tree.h"
#include "../common.h"

struct Transformation {
    Tree_node* orig = nullptr;
    Tree_node* diff = nullptr;
};