#include <assert.h>
#include <math.h>

#include "replacer.h"
#include "../common.h"
#include "../Sol_generating/latex.h"

const int Var_weight = 1;
const int Max_weight = 30;

const int To_be_replaced = -1;



static size_t calc_subtree_weight(Tree_node *head);

static void   calc_weights(Transformations *tranfs);
static void   calc_weights(Tree            *tree);

static bool   compare_subtrees(const Tree_node *head1, const Tree_node *head2);

static int    replace_node_everywhere(Tree_node *node, Transformations *transfs, Tree *tree1, 
                                                                                 Tree *tree2);

static void   replace_same_as_in(const Tree_node *source, Tree_node       *subtree);
static void   replace_same_as_in(const Tree_node *node,   Transformations *transfs);



void save_transf(Tree_node *orig, Tree_node *diff, Transformations *transf) {

    assert(orig   != nullptr);
    assert(diff   != nullptr);
    assert(transf != nullptr);

    transf->orig[transf->index] = orig;
    transf->diff[transf->index] = diff;

    ++(transf->index);
}

#define DO_IF(func, condition) \
        if (condition)         \
            func;

void make_replacings(Tree *tree, Transformations *transfs) {

    calc_weights(tree);
    calc_weights(transfs);

    replace_node_everywhere(tree->head->left, transfs, tree, nullptr);
}

void make_replacings(Tree *orig, Tree *diff) {

    calc_weights(orig);
    calc_weights(diff);

    replace_node_everywhere(orig->head->left, nullptr, orig, diff);
    replace_node_everywhere(diff->head->left, nullptr, orig, diff);
}

static size_t calc_subtree_weight(Tree_node *head) {

    assert(head != nullptr);

    if (head->left) {
        head->weight += calc_subtree_weight(head->left);
    }

    if (head->right) {
        head->weight += calc_subtree_weight(head->right);
    }

    switch (head->type) {
        case OP:
            head->weight = head->weight + head->data.op + 1;
            break;
        case VAL:
            head->weight = head->weight + (int) log10(abs(head->data.var) + 1) + 1;
            break;
        case VAR:
            head->weight = head->weight + Var_weight;
            break;
        default:
            break;
    }

    if (head->weight >= Max_weight) {
        head->weight  = Var_weight;
        head->replace = To_be_replaced;
    }

    return head->weight;
}

static void calc_weights(Transformations *tranfs) {

    for (int i = 0; i < tranfs->index; ++i) {
        calc_subtree_weight(tranfs->orig[i]);
        calc_subtree_weight(tranfs->diff[i]);
    }
}

static void calc_weights(Tree *tree) {

    calc_subtree_weight(tree->head->left);
}

static bool compare_subtrees(const Tree_node *head1, const Tree_node *head2) {

    if (head1 == nullptr) {
        if (head2 == nullptr) {
            return true;
        }
        return false;

    } else {
        if (head2 == nullptr) {
            return false;
        }
    }

    if (head1->type != head2->type) {
        return false;
    }

    switch (head1->type) {
        case VAL:
            return (head1->data.val == head2->data.val);

        case VAR:
            return (head1->data.var == head2->data.var);

        case OP:
            if (head1->data.op != head2->data.op) {
                return false;
            }
            break;

        default: break;
    }

    if (is_commutative(head1->data.op)) {
        if (compare_subtrees(head1->left,  head2->right) && 
            compare_subtrees(head1->right, head2->left)) {

            return true;
        }
    }

    return (compare_subtrees(head1->left,  head2->left) && 
            compare_subtrees(head1->right, head2->right));

}

static int replace_node_everywhere(Tree_node *node, Transformations *transfs, Tree *tree1, 
                                                                              Tree *tree2) {

    assert(node != nullptr);

    static int variable_counter = 0;

    if (node->left) {
        replace_node_everywhere(node->left,  transfs, tree1, tree2);
    }

    if (node->right) {
        replace_node_everywhere(node->right, transfs, tree1, tree2);
    }

    if (node->replace == To_be_replaced) {
        node->replace = variable_counter;

        ++variable_counter;

        latex_print_replacing(node, node->replace);

        DO_IF(replace_same_as_in(node, tree1->head->left), tree1   != nullptr);
        DO_IF(replace_same_as_in(node, tree2->head->left), tree2   != nullptr);
        DO_IF(replace_same_as_in(node, transfs),           transfs != nullptr);
    }

    return variable_counter;
}

static void replace_same_as_in(const Tree_node *source, Tree_node *subtree) {

    assert(source != nullptr && subtree != nullptr);

    if (subtree->weight == source->weight) {
        if (compare_subtrees(subtree, source)) {
            subtree->replace = source->replace;
        }
    }

    if (subtree->left) {
        replace_same_as_in(source, subtree->left);
    }

    if (subtree->right) {
        replace_same_as_in(source, subtree->right);
    }
}

static void replace_same_as_in(const Tree_node *node, Transformations *transfs) {
    
    for (int i = 0; i < transfs->index; ++i) {
        replace_same_as_in(node, transfs->orig[i]);
        replace_same_as_in(node, transfs->diff[i]);
    }
}

void free_transfs(Transformations *transfs, bool free_copies) {

    if (free_copies) {
        for (int i = 0; i < transfs->index; ++i) {
            free_node(transfs->orig[i]);
            free_node(transfs->diff[i]);
        }
    }

    free(transfs->orig);
    free(transfs->diff);
}
