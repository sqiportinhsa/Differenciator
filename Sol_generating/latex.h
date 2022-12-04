#include "../common.h"
#include "../Tree/tree.h"

void set_tex_output(FILE *output);

FILE* get_output_stream();

void print_to_latex(const char *message, ...);

void init_latex(const char *filename);

void close_latex();

void latex_print_subtree(Tree_node *head);