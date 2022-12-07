#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "simpl.h"

#include "../common.h"
#include "../Sol_generating/latex.h"

#include "../DSL.h"


static Tree_node* simplify_subtree (Tree_node* node);

static Tree_node* simplify_add     (Tree_node *node);

static Tree_node* simplify_sub     (Tree_node *node);

static Tree_node* simplify_mul     (Tree_node *node);

static Tree_node* simplify_deg     (Tree_node *node);


void simplify_tree(Tree *tree) {

    print_to_latex("\\section{Упрощение полученной формулы}"
                   "Почему-то все любят, когда просто, и никто не любит, когда сложно. Однако, "
                   "усложнять внезапно оказывается легко, \\sout{вы можете видеть это на примере "
                   "моего кода}, а упрощать сложно. Поэтому я сам сделаю это для вас, а вам "
                   "останется лишь наблюдать за этим прекрасным процессом. Приступим:\n\n");

    tree->head->left = simplify_subtree(tree->head->left);

    set_as_parent(tree->head->left);

    print_to_latex("Объединяя вышесказанное получим \\sout{неуд за таску} производную в упрощенном "
                   "виде:\n\n");

    latex_print_expr(tree->head->left);
}

static Tree_node* simplify_subtree(Tree_node* node) {
    if (node->left) {
        if (node->left->type == OP) {
            node->left = simplify_subtree(node->left);
        }
    }

    if (node->right) {
        if (node->right->type == OP) {
            node->right = simplify_subtree(node->right);
        }
    }

    set_as_parent(node);

    switch (node->data.op) {
        case ADD:
            return simplify_add(node);

        case SUB:
            return simplify_sub(node);

        case MUL:
            return simplify_mul(node);

        case DEG:
            return simplify_deg(node);

        case SIN: case TAN: case EXP: case DIV:
            REPLACE_VALUE_ON_ARG_TO(0, 0);

        case COS: case CTG:
            REPLACE_VALUE_ON_ARG_TO(0, 1);

        default:
            break;
    }

    return node;
}

static Tree_node* simplify_add(Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == ADD);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 0);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(ADD, +);

    return node;
}

static Tree_node* simplify_sub(Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == SUB);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(SUB, +);

    SUBTRACT_FROM_ZERO();

    return node;
}

static Tree_node* simplify_mul(Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == MUL);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 1);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  1);

    REPLACE_TWO_VALUES(MUL, *);

    MULTIPLY_BY_ZERO(node->left);
    MULTIPLY_BY_ZERO(node->right);

    return node;
}

static Tree_node* simplify_deg(Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == DEG);

    REPLACE_VALUE_ON_ARG_TO(0, 0);
    REPLACE_VALUE_ON_ARG_TO(1, 1);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left, 1);
    
    return node;
}
