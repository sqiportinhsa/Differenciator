#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "simpl.h"

#include "../common.h"
#include "../Sol_generating/latex.h"

#include "../DSL.h"

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                              STATIC FUNCTIONS DECLARATION                                      //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

static Tree_node* simplify_subtree (Tree_node *node, Transformations *transf);

static Tree_node* simplify_add     (Tree_node *node, Transformations *transf);
static Tree_node* simplify_sub     (Tree_node *node, Transformations *transf);
static Tree_node* simplify_mul     (Tree_node *node, Transformations *transf);
static Tree_node* simplify_deg     (Tree_node *node, Transformations *transf);


static Tree_node* skip_neutral_element      (Transformations *transf, Tree_node *node, 
                                                                      Tree_node *brother);
static Tree_node* subtract_from_zero        (Transformations *transf, Tree_node *node);
static Tree_node* replace_two_values        (Transformations *transf, Tree_node *node, int new_val);
static Tree_node* replace_value_on_arg_to   (Transformations *transf, Tree_node *node, int value);
static Tree_node* multiply_by_zero          (Transformations *transf, Tree_node *node);

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                  SIMPLIFYING MACRO                                             //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define SKIP_NEUTRAL_ELEMENT(checking, brother, elem)                              		           \
        if (checking->type == VAL && checking->data.val == elem)                   	               \
			return skip_neutral_element(transf, node, brother);                                    

#define SUBTRACT_FROM_ZERO()                                                                       \
		if (node->left->type == VAL && node->left->data.val == 0)                   	           \
			return subtract_from_zero(transf, node);

#define REPLACE_TWO_VALUES(op_code, oper)                                                          \
        if (node->left->type == VAL && node->right->type == VAL && node->data.op == op_code)       \
        	return replace_two_values(transf, node, node->left->data.val oper                      \
			                                       node->right->data.val);

#define REPLACE_VALUE_ON_ARG_TO(arg, value)                                                        \
    	if (node->left->type != OP && node->left->data.val == arg)                                 \
			return replace_value_on_arg_to(transf, node, value);

#define MULTIPLY_BY_ZERO(checking)                                                                 \
		if (checking->type == VAL && checking->data.val == 0)                                      \
			return multiply_by_zero(transf, node);


//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                SIMPLIFYING TREE & SUBTREE                                      //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

#define memory_allocate(ptr, size, type)                                                           \
        ptr = (type*) calloc(size, sizeof(type));                                                  \
        if (ptr == nullptr) {                                                                      \
            printf("can't allocate memory: not enought free mem\n");                               \
            return;                                                                                \
        }

void simplify_tree(Tree *tree) {

    print_to_latex("Добавим следующие замены, "
                   "\\sout{ведь вы совершенно точно помните старые}:\n\n");

    int transfs_amount = count_nodes(tree->head);

    Transformations transfs = {};
    
    memory_allocate(transfs.orig, transfs_amount, Tree_node*);
    memory_allocate(transfs.diff, transfs_amount, Tree_node*);

    tree->head = simplify_subtree(tree->head, &transfs);
    set_as_parent(tree->head);

    make_replacings(tree, &transfs);

    print_to_latex("Теперь произведем упрощение:\n\n");
    latex_print_simplifying(&transfs, transfs_amount);

    free_transfs(&transfs, true);

    print_to_latex("Объединяя вышесказанное получим \\sout{неуд за таску} производную "
                   "в упрощенном виде:\n\n");

    latex_print_expr(tree->head);
}

#undef memory_allocate

//---------------------------//

static Tree_node* simplify_subtree(Tree_node *node, Transformations *transf) {
    if (node->type != OP) {
        print_to_latex("Замена для единственной константы не требуется. Продолжим\n\n");
        return node;
    }

    if (node->left) {
        if (node->left->type == OP) {
            node->left = simplify_subtree(node->left, transf);
        }
    }

    if (node->right) {
        if (node->right->type == OP) {
            node->right = simplify_subtree(node->right, transf);
        }
    }

    set_as_parent(node);

    switch (node->data.op) {
        case ADD: return simplify_add(node, transf);

        case SUB: return simplify_sub(node, transf);

        case MUL: return simplify_mul(node, transf);

        case DEG: return simplify_deg(node, transf);

        case SIN: case TAN: case EXP: case DIV:
            REPLACE_VALUE_ON_ARG_TO(0, 0);

        case COS: case CTG:
            REPLACE_VALUE_ON_ARG_TO(0, 1);

        default: break;
    }

    return node;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                     SIMPLIFYING NODES                                          //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

//-----------------------------//
//          ADDITION          //
//---------------------------//

static Tree_node* simplify_add(Tree_node *node, Transformations *transf) {

    assert(node != nullptr);
    assert(node->data.op == ADD);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 0);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(ADD, +);

    return node;
}

//-----------------------------//
//        SUBTRACKING         //
//---------------------------//

static Tree_node* simplify_sub(Tree_node *node, Transformations *transf) {

    assert(node != nullptr);
    assert(node->data.op == SUB);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  0);

    REPLACE_TWO_VALUES(SUB, -);

    SUBTRACT_FROM_ZERO();

    return node;
}

//-----------------------------//
//      MULTIPLICATION        //
//---------------------------//

static Tree_node* simplify_mul(Tree_node *node, Transformations *transf) {

    assert(node != nullptr);
    assert(node->data.op == MUL);

    SKIP_NEUTRAL_ELEMENT(node->left,  node->right, 1);
    SKIP_NEUTRAL_ELEMENT(node->right, node->left,  1);

    REPLACE_TWO_VALUES(MUL, *);

    MULTIPLY_BY_ZERO(node->left);
    MULTIPLY_BY_ZERO(node->right);

    return node;
}

//-----------------------------//
//          DEGREE            //
//---------------------------//

static Tree_node* simplify_deg(Tree_node *node, Transformations *transf) {

    assert(node != nullptr);
    assert(node->data.op == DEG);

    REPLACE_VALUE_ON_ARG_TO(0, 1);

    SKIP_NEUTRAL_ELEMENT(node->right, node->left, 1);
    
    return node;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                     SIMPLIFYING CASES                                          //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

static Tree_node* skip_neutral_element(Transformations *transf, Tree_node *node, 
                                                                Tree_node *brother) {
	Tree_node *simplifyed = copy_subtree(brother);                                         
																					 	           
	save_transf(copy_subtree(node), copy_subtree(simplifyed), transf);                     
	free_node(node);                                                                       
																						           
	return simplifyed;
}

//---------------------------//

static Tree_node* subtract_from_zero(Transformations *transf, Tree_node *node) {
	Tree_node *simplifyed = create_node(MUL, copy_subtree(node->right), Const(-1));        
																			 	           
	save_transf(copy_subtree(node), copy_subtree(simplifyed), transf);                     
	free_node(node);                                                                       
																				           
	return simplifyed;
}

//---------------------------//

static Tree_node* replace_two_values(Transformations *transf, Tree_node *node, int new_val) {
	Tree_node *new_node = Const(new_val);
	
	save_transf(copy_subtree(node), copy_subtree(new_node), transf);             
	free_node(node);

	return new_node; 
}

//---------------------------//

static Tree_node* replace_value_on_arg_to(Transformations *transf, Tree_node *node, int value) {
	Tree_node *new_node = Const(value);

	save_transf(copy_subtree(node), copy_subtree(new_node), transf);                       
	free_node(node);      

    return new_node;                                                                                                                                               
}

//---------------------------//

static Tree_node* multiply_by_zero(Transformations *transf, Tree_node *node) {
	Tree_node *new_node = Const(0);         

	save_transf(copy_subtree(node), copy_subtree(new_node), transf);
	free_node(node);                   

    return new_node;
}
