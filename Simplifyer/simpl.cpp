#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "simpl.h"

#include "../common.h"
#include "../Sol_generating/latex.h"

#include "../DSL.h"


static Tree_node* simplify_add_sub (Tree_node *node);

static Tree_node* simplify_mul     (Tree_node *node);

static Tree_node* simplify_deg     (Tree_node *node);


Tree_node* simplify_subtree(Tree_node* node) {
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
        case ADD: case SUB:
            return simplify_add_sub(node);

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

static Tree_node* simplify_add_sub(Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == ADD || node->data.op == SUB);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 0);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(ADD, +);
    REPLACE_TWO_VALUES(SUB, -);

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
