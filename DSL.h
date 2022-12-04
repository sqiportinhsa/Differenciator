#ifndef DIFF_DSL
#define DIFF_DSL

#define CS copy_subtree(source)

#define DL diff_node(source->left)
#define DR diff_node(source->right)
#define CL copy_subtree(source->left)
#define CR copy_subtree(source->right)

#define Diff_const                          \
        dest = create_node(VAL, 0);         \
        latex_print_diff(source, dest);     \
        return dest;

#define Diff_var                            \
        dest = create_node(VAL, 1);         \
        latex_print_diff(source, dest);     \
        return dest;

#define Const(c) create_node(VAL, c)

#define Add(l, r) create_node(OP, ADD, l, r)
#define Sub(l, r) create_node(OP, SUB, l, r)
#define Mul(l, r) create_node(OP, MUL, l, r)
#define Div(l, r) create_node(OP, DIV, l, r)

#define Deg(l, r) create_node(OP, DEG, l, r)

#define Square(l) create_node(OP, DEG, l, Const(2))

#define Cos(l) create_node(OP, COS, l)
#define Sin(l) create_node(OP, SIN, l)
#define Tan(l) create_node(OP, TAN, l)
#define Ctg(l) create_node(OP, CTG, l)

#define Exp(l) create_node(OP, EXP, l)

#define Log(l) create_node(OP, LOG, l)

#define Ret_dest(calc_func)                 \
        dest = calc_func;                   \
        set_as_parent(dest);                \
        latex_print_diff(source, dest);     \
        return dest;

#endif
