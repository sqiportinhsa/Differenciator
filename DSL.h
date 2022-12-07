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

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                        NODE CREATING                                           //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


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

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                        SIMPLIFYING                                             //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define SKIP_NEUTRAL_ELEMENT(checking, brother, elem)                              \
        if (checking->type == VAL && checking->data.val == elem) {                 \
        Tree_node *simplifyed = brother;                                           \
                                                                                   \
        latex_print_simplify(node, brother);                                       \
                                                                                   \
        free(checking);                                                            \
        free(node);                                                                \
                                                                                   \
        return simplifyed;                                                         \
    }

#define REPLACE_TWO_VALUES(op_code, oper)                                                          \
        if (node->left->type == VAL && node->right->type == VAL) {                                 \
        	if (node->data.op == op_code) {                                                        \
				Tree_node *new_node = Const(node->left->data.val oper node->right->data.val);      \
				latex_print_simplify(node, new_node);                                              \
				free_node(node);                                                                   \
				return new_node;                                                                   \
        	}                                                                                      \
    	}

#define REPLACE_VALUE_ON_ARG_TO(arg, value)               \
    	if (node->left->data.val == arg) {                \
			Tree_node *new_node = Const(value);           \
    	    latex_print_simplify(node, new_node);         \
    	    free_node(node);                              \
    	    return new_node;                              \
    	}                                                 \
    	return node;

#define MULTIPLY_BY_ZERO(checking)                        \
		if (checking->data.val == 0) {                    \
			Tree_node *new_node = Const(0);               \
    	    latex_print_simplify(node, new_node);         \
    	    free_node(node);                              \
    	    return new_node;                              \
    	}                                                 \

#endif
