#ifndef DIF_COMMON_H
#define DIF_COMMON_H

#include "Tree/tree.h"
#include "Libs/file_reading.hpp"

static const char *default_input  = "example.txt";
static const char *default_output = "GDZ_matematica_5_class.pdf";

struct Expression {
    Tree  origin          = {};
    Tree  first_deg       = {};
    char* text_origin     = nullptr;
};

bool init_expression(Expression *expr, const char *input);

CLArgs get_filenames(int argc, const char **argv);

void expr_dtor(Expression *expr);

bool is_commutative(Operations op);

#endif