#include "../common.h"
#include "../Tree/tree.h"

void set_tex_output(FILE *output);

FILE* get_output_stream();

void print_to_latex(const char *message, ...);

void init_latex(const char *filename);

void close_latex();

void latex_print_expr(Tree_node *head);

void latex_print_diff(const Tree_node *orig, const Tree_node *diff);

void latex_print_simplify(const Tree_node *orig, const Tree_node *simpl);

void latex_print_phrase();

void print_diff_introduction(Tree *tree);