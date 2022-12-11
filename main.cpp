#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "Diff/diff.h"
#include "Sol_generating/latex.h"
#include "Simplifyer/simpl.h"

int main(int argc, const char **argv) {

    srand((unsigned int)time(NULL));

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    init_expression(&expr, filenames.input);

    init_latex(filenames.output);

    print_introduction(&expr.origin);

    diff_tree(&expr.origin, &expr.first_deg);

    simplify_tree(&expr.first_deg);

    close_latex();

    expr_dtor(&expr);

    system("pdflatex latex_output.tex");

    return 0;
}