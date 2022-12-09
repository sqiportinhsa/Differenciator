#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "simpl.h"

#include "../common.h"
#include "../Sol_generating/latex.h"

#include "../DSL.h"


static Tree_node* simplify_subtree (Tree_node* node, Transformation **transf);

static Tree_node* simplify_add     (Tree_node *node, Transformation **transf);

static Tree_node* simplify_sub     (Tree_node *node, Transformation **transf);

static Tree_node* simplify_mul     (Tree_node *node, Transformation **transf);

static Tree_node* simplify_deg     (Tree_node *node, Transformation **transf);


#define memory_allocate(ptr, size, type)                                                      \
        type *ptr = (type*) calloc(size, sizeof(type));                                       \
        if (ptr == nullptr) {                                                                 \
            printf("can't allocate memory: not enought free mem\n");                          \
            return;                                                                           \
        }

void simplify_tree(Tree *tree) {

    print_to_latex("\\section{Упрощение полученной формулы}"
                   "Почему-то все любят, когда просто, и никто не любит, когда сложно. Однако, "
                   "усложнять внезапно оказывается легко, \\sout{вы можете видеть это на примере "
                   "моего кода}, а упрощать сложно. Поэтому я сам сделаю это для вас, а вам "
                   "останется лишь наблюдать за этим прекрасным процессом. Приступим:\n\n");

    int transfs_amount = count_nodes(tree->head);
    
    memory_allocate(transfs, transfs_amount, Transformation);

    tree->head->left = simplify_subtree(tree->head->left, &transfs);

    set_as_parent(tree->head->left);

    latex_print_simplifying(transfs, transfs_amount);

    print_to_latex("Объединяя вышесказанное получим \\sout{неуд за таску} производную в упрощенном "
                   "виде:\n\n");

    latex_print_expr(tree->head->left);
}

#undef memory_allocate

static Tree_node* simplify_subtree(Tree_node* node, Transformation **transf) {
    if (node->left) {
        if (node->left->type == OP) {
            node->left = simplify_subtree(node->left, transf);
        }
    }

    if (node->right) {
        if (node->right->type == OP) {
            node->right = simplify_subtree(node->right, transf);
        }
    }

    set_as_parent(node);

    switch (node->data.op) {
        case ADD:
            return simplify_add(node, transf);

        case SUB:
            return simplify_sub(node, transf);

        case MUL:
            return simplify_mul(node, transf);

        case DEG:
            return simplify_deg(node, transf);

        case SIN: case TAN: case EXP: case DIV:
            REPLACE_VALUE_ON_ARG_TO(0, 0);

        case COS: case CTG:
            REPLACE_VALUE_ON_ARG_TO(0, 1);

        default:
            break;
    }

    return node;
}

static Tree_node* simplify_add(Tree_node *node, Transformation **transf) {

    assert(node != nullptr);
    assert(node->data.op == ADD);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 0);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(ADD, +);

    return node;
}

static Tree_node* simplify_sub(Tree_node *node, Transformation **transf) {

    assert(node != nullptr);
    assert(node->data.op == SUB);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(SUB, +);

    SUBTRACT_FROM_ZERO();

    return node;
}

static Tree_node* simplify_mul(Tree_node *node, Transformation **transf) {

    assert(node != nullptr);
    assert(node->data.op == MUL);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 1);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  1);

    REPLACE_TWO_VALUES(MUL, *);

    MULTIPLY_BY_ZERO(node->left);
    MULTIPLY_BY_ZERO(node->right);

    return node;
}

static Tree_node* simplify_deg(Tree_node *node, Transformation **transf) {

    assert(node != nullptr);
    assert(node->data.op == DEG);

    REPLACE_VALUE_ON_ARG_TO(0, 0);
    REPLACE_VALUE_ON_ARG_TO(1, 1);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left, 1);
    
    return node;
}
