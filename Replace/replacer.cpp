#include <assert.h>
#include <math.h>

#include "replacer.h"
#include "../common.h"
#include "../Sol_generating/latex.h"

const int Var_weight = 1;
const int Max_weight = 10;

const int To_be_replaced = -1;



static size_t calc_subtree_weight(Tree_node *head);

static bool   compare_subtrees(const Tree_node *head1, const Tree_node *head2);

static int replace_node_in_trees(Tree_node *node, Tree *tree1, Tree *tree2);

static void   replace_same_as_in(const Tree_node *source, Tree_node *subtree);



void save_transf(Tree_node *orig, Tree_node *diff, Transformations *transf) {

    assert(orig   != nullptr);
    assert(diff   != nullptr);
    assert(transf != nullptr);

    transf->orig[transf->index] = copy_subtree(orig);
    transf->diff[transf->index] = copy_subtree(diff);

    ++(transf->index);
}

#define DO_IF(func, condition) \
        if (condition)         \
            func;

void make_replacings(Tree *orig, Tree *diff) {

    DO_IF(calc_subtree_weight(orig->head->left), orig != nullptr);
    DO_IF(calc_subtree_weight(diff->head->left), diff != nullptr);

    DO_IF(replace_node_in_trees(orig->head->left, orig, diff), orig != nullptr);
    DO_IF(replace_node_in_trees(diff->head->left, orig, diff), diff != nullptr);
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

static int replace_node_in_trees(Tree_node *node, Tree *tree1, Tree *tree2) {

    assert(node != nullptr);

    static int variable_counter = 0;

    if (node->left) {
        replace_node_in_trees(node->left, tree1, tree2);
    }

    if (node->right) {
        replace_node_in_trees(node->right, tree1, tree2);
    }

    if (node->replace == To_be_replaced) {
        node->replace = variable_counter;

        ++variable_counter;

        latex_print_replacing(node, node->replace);

        DO_IF(replace_same_as_in(node, tree1->head->left), tree1 != nullptr);
        DO_IF(replace_same_as_in(node, tree2->head->left), tree2 != nullptr);
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

void free_transfs(Transformations *transfs) {

    for (int i = 0; i < transfs->index; ++i) {
        free_node(transfs->orig[i]);
        free_node(transfs->diff[i]);
    }

    free(transfs->orig);
    free(transfs->diff);
}
