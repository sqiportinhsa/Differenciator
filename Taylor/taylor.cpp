#include <stdio.h>

#include "taylor.h"

#include "../Sol_generating/latex.h"
#include "../Diff/diff.h"
#include "../Simplifyer/simpl.h"
#include "../DSL.h"

const int num_of_derivatives = 3;

static void calc_derivative(Tree *prev_der, Tree *taylor, int i);
static int  factorial(int num);


void taylor(Expression *expr) {

    print_to_latex("\\section{Тейлор}\n\n"
                   "Сначала нам потребуется вычислить оставшиеся производные вплоть до %d-го "
                   "порядка.\n\n", num_of_derivatives);

    expr->taylor.head = Add(copy_subtree(expr->origin.head), 
                            Mul(Div(copy_subtree(expr->derivative.head), Const(1)), 
                                Sub(Variable('y'), Variable('x'))));

    for (int i = 2; i <= num_of_derivatives; ++i) {
        calc_derivative(&expr->derivative, &expr->taylor, i);
    }

    print_to_latex("\\subsection{Итоговый результат}\n\n"
                   "Произведем необходимые замены:");

    make_replacings(&expr->taylor);

    print_to_latex("Таким образом получается следующее разложение в точке y вблизи x:");

    latex_print_expr(expr->taylor.head);
}

static void calc_derivative(Tree *prev_der, Tree *taylor, int i) {

    print_to_latex("\\subsection{Вычисление %d-ой производной}\n\n"
                   "\\subsubsection{Дифференцирование}", i);

    Tree next_der = {};
    init_tree(&next_der);

    diff_tree(prev_der, &next_der);

    print_to_latex("\\subsubsection{Упрощение производной}");

    simplify_tree(&next_der);

    taylor->head = Add(taylor->head, 
                       Mul(Div(copy_subtree(next_der.head), Const(factorial(i))), 
                           Deg(Sub(Variable('y'), Variable('x')), Const(i))));

    free_node(prev_der->head);
    prev_der->head = copy_subtree(next_der.head);

    tree_dtor(&next_der);
}

static int factorial(int num) {
    int res  = 1;

    for (int i = 1; i <= num; ++i) {
        res *= i;
    }

    return res;
}