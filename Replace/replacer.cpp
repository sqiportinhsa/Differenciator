#include <assert.h>
#include <math.h>

#include "replacer.h"
#include "../common.h"
#include "../Sol_generating/latex.h"

const int Var_weight = 1;
const int Max_weight = 10000;

const int To_be_replaced = -1;

void save_transf(Tree_node *orig, Tree_node *diff, Transformation **transf) {

    assert(orig   != nullptr);
    assert(diff   != nullptr);
    assert(transf != nullptr);

    (*transf)->orig = orig;
    (*transf)->diff = diff;

    ++(*transf);
}

size_t calc_subtree_weight(Tree_node *head) {

    assert(head != nullptr);

    if (head->weight != 0) {
        return head->weight;
    }

    if (head->left) {
        head->weight += calc_subtree_weight(head->left);
    }

    if (head->right) {
        head->weight += calc_subtree_weight(head->right);
    }

    switch (head->type) {
        case OP:
            head->weight = head->weight * 33 + head->data.op + 1;
            break;
        case VAL:
            head->weight = head->weight * 33 + (int) log10(head->data.var) + 1;
            break;
        case VAR:
            head->weight = head->weight * 33 + Var_weight;
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

bool compare_subtrees(const Tree_node *head1, const Tree_node *head2) {

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

int make_replacings(Tree_node *head, Tree_node *node) {

    assert(head != nullptr && node != nullptr);

    static int variable_counter = 0;

    if (node->replace == To_be_replaced) {
        node->replace = variable_counter;

        ++variable_counter;

        latex_print_replacing(node, variable_counter);

        replace_same_in_subtree(node, head);
    }

    if (node->left) {
        make_replacings(head, node->left);
    }

    if (node->right) {
        make_replacings(head, node->right);
    }

    return variable_counter;
}

void replace_same_in_subtree(const Tree_node *source, Tree_node *subtree) {

    assert(source != nullptr && subtree != nullptr);

    if (subtree->weight == source->weight) {
        if (compare_subtrees(subtree, source)) {
            subtree->replace = source->replace;
        }
    }

    if (subtree->left) {
        replace_same_in_subtree(source, subtree);
    }

    if (subtree->right) {
        replace_same_in_subtree(source, subtree);
    }
}
