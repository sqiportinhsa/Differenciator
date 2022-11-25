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

    retun true;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                     DIFFERENCIATING SECTION                                    //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define DL diff_node(source->left)
#define DR diff_node(source->right)
#define CR copy_tree(source->left)
#define CL copy_left(source->right)

#define diff_const fill_node(VAL, 0)

#define deff_var fill_node(VAL, 1)

#define Add(l, r) fill_node(op, ADD, l, r)
#define Sub(l, r) fill_node(op, SUB, l, r)
#define Mul(l, r) fill_node(op, MUL, l, r)
#define Div(l, r) fill_node(op, DIV, l, r)

#define Square(l) fill_node(op, MUL, l, l);

#define Cos(l) fill_node(op, COS, l)
#define Sin(l) fill_node(op, SIN, l)
#define Tan(l) fill_node(op, TAN, l)
#define Ctg(l) fill_node(op, CTG, l)

#define Exp(l) fill_node(op, EXP, l)

#define Deg(l, r) fill_node(op, DEG, l, r)

void make_diff_tree(Expression *expr) {


}

Tree_node* diff_node(Tree_node *source) {

    if (source->type == VAL) {

        return fill_node(VAL, 0);
    }

    if (source->type == VAR) {

        return fill_node(VAL, 1);
    }

    dest->type = OP;

    switch (source->data.op) {
        case ADD: return Add(DL, DR);
        case SUB: return Sub(DL, DR);
        case MUL: return Add(Mul(DL, CR), Mul(CL, DR));
        case DIV: return Sub(Div(DL, CR), Div(Mul(CL, DR), Mul(CR, CR)));
        case SIN: return Mul(Cos(CL), DL);
        case COS: return Mul(Mul(Sin(CL), -1), DL);
        case TAN: return Mul(Div(1, Square(Cos(CL))), DL);
        case CTG: return Mul(Div(1, Square(Sin(CL))), Mul(DL, -1));
        case EXP: return Mul(Exp(CL), DL);
        case LOG: return Div(DL, CL);
        case DEG: return Exp()
    }
}
