#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "Diff/diff.h"
#include "Sol_generating/latex.h"

int main(int argc, const char **argv) {

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    init_expression(&expr, filenames.input);

    init_latex(filenames.output);

    dump_tree(&expr.origin, "origin");

    diff_tree(&expr.origin, &expr.first_deg);

    close_latex();

    expr_dtor(&expr);

    return 0;
}