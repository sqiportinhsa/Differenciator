#include <stdio.h>

#include "common.h"
#include "Diff/diff.h"

int main(int argc, const char **argv) {

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    init_expression(&expr, &filenames);

    dump_tree(&expr.origin, "origin\n");

    diff_tree(&expr.origin, &expr.diffirenciated);

    dump_tree(&expr.origin, "origin\n");
    dump_tree(&expr.diffirenciated, "diff\n")

    expr_dtor(&expr);

    return 0;
}