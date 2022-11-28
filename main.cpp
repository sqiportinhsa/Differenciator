#include <stdio.h>

#include "common.h"
#include "Diff/diff.h"

int main(int argc, const char **argv) {

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    init_expression(&expr, &filenames);

    dump_tree(&expr.origin, "origin\n");

    printf("head node: %d. its children right %d and left %d\n", expr.origin.head->data.op, expr.origin.head->right->data.val, expr.origin.head->left->data.val);

    diff_tree(&expr.origin, &expr.diffirenciated);

    dump_tree(&expr.origin, "origin\n");
    dump_tree(&expr.diffirenciated, "diff\n")

    expr_dtor(&expr);

    return 0;
}