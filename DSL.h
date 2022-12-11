#ifndef DIFF_DSL
#define DIFF_DSL

#define CS copy_subtree(source)

#define DL diff_node(source->left,  transf)
#define DR diff_node(source->right, transf)
#define CL copy_subtree(source->left)
#define CR copy_subtree(source->right)

#define Diff_const                          \
        dest = create_node(0);              \
        save_transf(source, dest, transf);  \
        return dest;

#define Diff_var                            \
        dest = create_node(1);              \
        save_transf(source, dest, transf);  \
        return dest;

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                        NODE CREATING                                           //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define Const(c) create_node(c)

#define Add(l, r) create_node(ADD, l, r)
#define Sub(l, r) create_node(SUB, l, r)
#define Mul(l, r) create_node(MUL, l, r)
#define Div(l, r) create_node(DIV, l, r)

#define Deg(l, r) create_node(DEG, l, r)

#define Square(l) create_node(DEG, l, Const(2))

#define Cos(l) create_node(COS, l)
#define Sin(l) create_node(SIN, l)
#define Tan(l) create_node(TAN, l)
#define Ctg(l) create_node(CTG, l)

#define Exp(l) create_node(EXP, l)

#define Log(l) create_node(LOG, l)

#define Ret_dest(calc_func)                 \
        dest = calc_func;                   \
        set_as_parent(dest);                \
        save_transf(source, dest, transf);  \
        return dest;

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                        SIMPLIFYING                                             //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define SKIP_NEUTRAL_ELEMENT(checking, brother, elem)                              		           \
        if (checking->type == VAL && checking->data.val == elem) {                 	               \
			Tree_node *simplifyed = copy_subtree(brother);                                         \
																					 	           \
			save_transf(copy_subtree(node), copy_subtree(simplifyed), transf);                     \
			free_node(node);                                                                       \
																						           \
			return simplifyed;                                                                     \
    	}

#define SUBTRACT_FROM_ZERO()                                                                       \
		if (node->left->type == VAL && node->left->data.val == 0) {                 	           \
			Tree_node *simplifyed = create_node(MUL, copy_subtree(node->right), Const(-1));        \
																					 	           \
			save_transf(copy_subtree(node), copy_subtree(simplifyed), transf);                     \
			free_node(node);                                                                       \
																						           \
			return simplifyed;                                                                     \
    	}      

#define REPLACE_TWO_VALUES(op_code, oper)                                                          \
        if (node->left->type == VAL && node->right->type == VAL) {                                 \
        	if (node->data.op == op_code) {                                                        \
				Tree_node *new_node = Const(node->left->data.val oper node->right->data.val);      \
				save_transf(copy_subtree(node), copy_subtree(new_node), transf);                   \
				free_node(node);                                                                   \
				return new_node;                                                                   \
        	}                                                                                      \
    	}

#define REPLACE_VALUE_ON_ARG_TO(arg, value)                                                        \
    	if (node->left->data.val == arg) {                                                         \
			Tree_node *new_node = Const(value);                                                    \
			save_transf(copy_subtree(node), copy_subtree(new_node), transf);                       \
			free_node(node);                                                                       \
    	    return new_node;                                                                       \
    	}                                                                                          \
    	return node;

#define MULTIPLY_BY_ZERO(checking)                                                                 \
		if (checking->data.val == 0) {                                                             \
			Tree_node *new_node = Const(0);                                                        \
			save_transf(copy_subtree(node), copy_subtree(new_node), transf);                       \
			free_node(node);                                                                       \
    	    return new_node;                                                                       \
    	}                                                                                          \

#endif
