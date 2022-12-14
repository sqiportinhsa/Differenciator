#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "common.h"
#include "Diff/diff.h"
#include "Sol_generating/latex.h"
#include "Simplifyer/simpl.h"
#include "Taylor/taylor.h"

int main(int argc, const char **argv) {

    srand((unsigned int)time(NULL));

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    if (!init_expression(&expr, filenames.input)) {
        expr_dtor(&expr);
        return -1;
    }

    init_latex(filenames.output);

    print_introduction(&expr.origin);

    differenciation (&expr);
    taylor          (&expr);

    close_latex();

    expr_dtor(&expr);

    system("pdflatex latex_output.tex");

    return 0;
}