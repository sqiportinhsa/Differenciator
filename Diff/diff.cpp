#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "diff.h"
#include "../Libs/file_reading.hpp"
#include "../Tree/tree.h"
#include "../Rec_desc/descent.h"
#include "../Sol_generating/latex.h"
#include "../Simplifyer/simpl.h"

#include "../DSL.h"

static Tree_node* diff_deg(Tree_node *source, Transformations *transf);


#ifdef DEBUG

#define DEBUG_PRINT(str, ...) {\
    printf ("Func: %20s  line %3d " str, __func__, __LINE__, ##__VA_ARGS__);\
}

#else 

#define DEBUG_PRINT(str, ...) ;

#endif


#define memory_allocate(ptr, size, type)                                                      \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            printf("can't allocate memory: not enought free mem\n");                          \
            return;                                                                           \
        }

void differenciation(Expression *expr) {
    print_to_latex("\\section{Дифференцирование}\n\n"
                   "\\subsection{Взятие производной}\n\n");

    diff_tree(&expr->origin, &expr->derivative);

    print_to_latex("Вы ещё не утомились? Самое время взять чашечку чая и печеньки, потому что мы "
                   "переходим к следующему этапу работы с выражением\n\n");

    print_to_latex("\\subsection{Упрощение полученной формулы}\n\n");

    simplify_tree(&expr->derivative);
}

void diff_tree(Tree *source, Tree *dest) {

    int transfs_amount = count_nodes(source->head);

    print_to_latex("Cначала проведем следующие замены:\n\n");

    Transformations transfs = {};
    
    memory_allocate(transfs.orig, transfs_amount, Tree_node*);
    memory_allocate(transfs.diff, transfs_amount, Tree_node*);

    dest->head = diff_node(source->head, &transfs);
    make_replacings(source, dest);

    print_to_latex("После чего мы готовы приступить непосредственно к дифференцированию:\n\n");

    latex_print_differenciation(&transfs, transfs_amount);

    free_transfs(&transfs);

    print_to_latex("Таким образом получаем следующую производную:\n\n");

    latex_print_expr(dest->head);
}

#undef memory_allocate

Tree_node* diff_node(Tree_node *source, Transformations *transf) {

    assert(source != nullptr);

    Tree_node *dest = nullptr;

    if (source->type == VAL) {
        DEBUG_PRINT("diff value node.    val: %d\n", source->data.val);
        return Diff_const;
    }

    if (source->type == VAR) {
        DEBUG_PRINT("diff variable node. var: %d\n", source->data.var);
        return Diff_var;
    }

    DEBUG_PRINT("diff operation node. op: %d\n", source->data.op);

    switch (source->data.op) {
        case ADD: Ret_dest(Add(DL, DR));
        case SUB: Ret_dest(Sub(DL, DR));
        case MUL: Ret_dest(Add(Mul(DL, CR), Mul(CL, DR)));
        case DIV: Ret_dest(Sub(Div(DL, CR), Div(Mul(CL, DR), Square(CR))));
        case SIN: Ret_dest(Mul(Cos(CL), DL));
        case COS: Ret_dest(Mul(Mul(Sin(CL), Const(-1)), DL));
        case TAN: Ret_dest(Mul(Div(Const(1), Square(Cos(CL))), DL));
        case CTG: Ret_dest(Mul(Div(Const(1), Square(Sin(CL))), Mul(DL, Const(-1))));
        case EXP: Ret_dest(Mul(Exp(CL), DL));
        case LOG: Ret_dest(Div(DL, CL));

        case DEG: return diff_deg(source, transf);
        
        default:
            assert(0 && "unexpected op case");
            break;
    }

    return nullptr;
}

static Tree_node* diff_deg(Tree_node *source, Transformations *transf) {
    assert(source != nullptr && transf != nullptr);
    assert(source->data.op == DEG);

    Tree_node *dest = nullptr;

    if (source->left->type == VAL) {
        if (source->right->type == VAL) {
            return Diff_const;

        } else {
            Ret_dest(Mul(Mul(CS, Log(CL)), DR));
        }

    } else {
        if (source->right->type == VAL) {
            Ret_dest(Mul(Mul(CR, Deg(CL, Sub(CR, Const(1)))), DL));
        } else {
            Ret_dest(Mul(CS, Add(Div(Mul(CR, DL), CL), Mul(DR, Log(CL)))));
        }
    }

    return nullptr;
}
