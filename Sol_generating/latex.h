#include "../common.h"
#include "../Tree/tree.h"
#include "../Replace/replacer.h"

void set_tex_output(FILE *output);

FILE* get_output_stream();

void print_to_latex(const char *message, ...);

void init_latex(const char *filename);

void close_latex();

void latex_print_expr(Tree_node *head);

void latex_print_diff(const Transformation *transf);

void latex_print_simplify(const Transformation *transf);

void latex_print_replacing(const Tree_node *node, int number);

void latex_print_phrase();

void print_introduction(Tree *tree);