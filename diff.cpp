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

    expr->origin.head = create_orphan_node();

    if (!descent(expr->text_origin, expr->origin.head)) {

        return false;
    }

    return true;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                     DIFFERENCIATING SECTION                                    //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define CS copy_subtree(source)

#define DL diff_node(source->left)
#define DR diff_node(source->right)
#define CR copy_subtree(source->left)
#define CL copy_subtree(source->right)

#define Diff_const create_node(VAL, 0)

#define Diff_var create_node(VAL, 1)

#define Const(c) create_node(VAL, c)

#define Add(l, r) create_node(OP, ADD, l, r)
#define Sub(l, r) create_node(OP, SUB, l, r)
#define Mul(l, r) create_node(OP, MUL, l, r)
#define Div(l, r) create_node(OP, DIV, l, r)

#define Deg(l, r) create_node(OP, DEG, l, r)

#define Square(l) create_node(OP, MUL, l, l)

#define Cos(l) create_node(OP, COS, l)
#define Sin(l) create_node(OP, SIN, l)
#define Tan(l) create_node(OP, TAN, l)
#define Ctg(l) create_node(OP, CTG, l)

#define Exp(l) create_node(OP, EXP, l)

#define Log(l) create_node(OP, LOG, l)


void diff_tree(Tree *source, Tree *dest) {

    source->head = diff_node(dest->head);


}

Tree_node* diff_node(Tree_node *source) {

    assert(source != nullptr);

    if (source->type == VAL) {

        return Diff_const;
    }

    if (source->type == VAR) {

        return Diff_var;
    }

    Tree_node *dest = create_empty_node();

    switch (source->data.op) {
        case ADD: return Add(DL, DR);
        case SUB: return Sub(DL, DR);
        case MUL: return Add(Mul(DL, CR), Mul(CL, DR));
        case DIV: return Sub(Div(DL, CR), Div(Mul(CL, DR), Mul(CR, CR)));
        case SIN: return Mul(Cos(CL), DL);
        case COS: return Mul(Mul(Sin(CL), Const(-1)), DL);
        case TAN: return Mul(Div(Const(1), Square(Cos(CL))), DL);
        case CTG: return Mul(Div(Const(1), Square(Sin(CL))), Mul(DL, Const(-1)));
        case EXP: return Mul(Exp(CL), DL);
        case LOG: return Div(DL, CL);
        case DEG: return Mul(CS, Add(Div(Mul(CR, DL), CL), Mul(DR, Log(CL))));
    }
}

Tree_node* copy_subtree(Tree_node *source) {

    assert(source != nullptr);

    Tree_node *copy = create_empty_node();

    copy->data = source->data;
    copy->type = source->type;

    copy->left  = copy_subtree(source->left);
    copy->right = copy_subtree(source->right);

}
