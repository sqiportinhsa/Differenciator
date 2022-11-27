#include <stdio.h>

#include "common.h"

int main(int argc, const char **argv) {

    CLArgs filenames = get_filenames(argc, argv);

    Expression expr = {};

    init_expression(&expr, &filenames);

    generate_book(&expr);

    expr_dtor(&expr);

    return 0;
}