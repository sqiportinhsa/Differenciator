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

bool init_expression(Expression *expr, const char *input) {
    
    assert(expr      != nullptr);

    expr->text_origin = get_input(input);

    init_tree(&expr->origin);
    init_tree(&expr->first_deg);

    expr->origin.head = create_orphan_node(); //fictive head element without data

    expr->origin.head->left = create_empty_node(expr->origin.head); //first tree element

    if (!descent(expr->text_origin, expr->origin.head->left)) {

        return false;
    }

    expr->first_deg.head = create_orphan_node();

    expr->first_deg.head->left = create_empty_node(expr->first_deg.head);

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
    tree_dtor(&expr->first_deg);
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
