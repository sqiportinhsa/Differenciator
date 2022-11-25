#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "diff.h"
#include "Libs/file_reading.hpp"
#include "Tree/tree.h"
#include "Rec_desc/descent.h"

#define memory_allocate(ptr, size, type, returning)                                           \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            tree_dtor(tree);                                                                  \
            return returning;                                                                 \
        }

bool init_formula(Expression *expr, char *text_or) {
    
    assert(expr != nullptr);

    expr->text_origin = text_or;

    init_tree(&expr->origin);
    init_tree(&expr->diffirenciated);

    expr->origin.head = create_head_node();

    if (!descent(expr->text_origin, expr->origin.head)) {

        return false;
    }

    return true;
}

