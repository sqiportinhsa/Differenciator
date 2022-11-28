#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "diff.h"
#include "../Libs/file_reading.hpp"
#include "../Tree/tree.h"
#include "../Rec_desc/descent.h"

#include "../DSL.h"

static Tree_node* diff_node(Tree_node *source);

static Tree_node* copy_subtree(Tree_node *source);


void diff_tree(Tree *source, Tree *dest) {

    source->head = diff_node(dest->head);

}

static Tree_node* diff_node(Tree_node *source) {

    assert(source != nullptr);

    if (source->type == VAL) {

        return Diff_const;
    }

    if (source->type == VAR) {

        return Diff_var;
    }

    Tree_node *dest = create_empty_node();

    switch (source->data.op) {
        case ADD: Ret_dest(Add(DL, DR));
        case SUB: Ret_dest(Sub(DL, DR));
        case MUL: Ret_dest(Add(Mul(DL, CR), Mul(CL, DR)));
        case DIV: Ret_dest(Sub(Div(DL, CR), Div(Mul(CL, DR), Mul(CR, CR))));
        case SIN: Ret_dest(Mul(Cos(CL), DL));
        case COS: Ret_dest(Mul(Mul(Sin(CL), Const(-1)), DL));
        case TAN: Ret_dest(Mul(Div(Const(1), Square(Cos(CL))), DL));
        case CTG: Ret_dest(Mul(Div(Const(1), Square(Sin(CL))), Mul(DL, Const(-1))));
        case EXP: Ret_dest(Mul(Exp(CL), DL));
        case LOG: Ret_dest(Div(DL, CL));
        case DEG: Ret_dest(Mul(CS, Add(Div(Mul(CR, DL), CL), Mul(DR, Log(CL)))));
        
        default:
            assert(0 && "unexpected op case");
            break;
    }

    return nullptr;
}

static Tree_node* copy_subtree(Tree_node *source) {

    assert(source != nullptr);

    Tree_node *copy = create_empty_node();

    copy->data = source->data;
    copy->type = source->type;

    copy->left  = copy_subtree(source->left);
    copy->right = copy_subtree(source->right);

    return copy;
}
