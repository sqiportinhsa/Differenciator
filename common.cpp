#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "common.h"

#include "Tree/tree.h"
#include "Rec_desc/descent.h"
#include "Libs/file_reading.hpp"

static char* get_input(const char *filename);

const int barrier_size = 3;


#define memory_allocate(ptr, size, type, returning)                                           \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            return returning;                                                                 \
        }

bool init_expression(Expression *expr, CLArgs *filenames) {
    
    assert(expr      != nullptr);
    assert(filenames != nullptr);

    expr->input_fn  = filenames->input;
    expr->output_fn = filenames->output;

    expr->text_origin = get_input(filenames->input);

    init_tree(&expr->origin);
    init_tree(&expr->diffirenciated);

    expr->origin.head = create_orphan_node();

    if (!descent(expr->text_origin, expr->origin.head)) {

        return false;
    }

    return true;
}

CLArgs get_filenames(int argc, const char **argv) {

    CLArgs args = parse_cmd_line(argc, argv);

    if (args.input == nullptr) {

        args.input = default_input;
    }

    if (args.output == nullptr) {

        args.output = default_output;
    }

    return args;
}

void expr_dtor(Expression *expr) {

    free(expr->text_origin);

    tree_dtor(&expr->origin);
    tree_dtor(&expr->diffirenciated);
}

static char* get_input(const char *filename) {

    size_t len = count_elements_in_file(filename);

    char *input = nullptr;

    memory_allocate(input, len + barrier_size, char, nullptr);

    read_file(input, len, filename);

    //set barrier

    input[len + 0] = '\0';
    input[len + 1] = '\0';
    input[len + 2] = '\0';    

    return input;
}
