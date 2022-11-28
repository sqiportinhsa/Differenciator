#ifndef DIF_COMMON_H
#define DIF_COMMON_H

#include "Tree/tree.h"
#include "Libs/file_reading.hpp"

static const char *default_input  = "example.txt";
static const char *default_output = "GDZ_matematica_5_class.pdf";

struct Expression {
    Tree  origin          = {};
    Tree  diffirenciated  = {};
    char* text_origin     = nullptr;
    const char* input_fn  = nullptr;
    const char* output_fn = nullptr;
};

bool init_expression(Expression *expr, CLArgs *filenames);

CLArgs get_filenames(int argc, const char **argv);

void expr_dtor(Expression *expr);

#endif