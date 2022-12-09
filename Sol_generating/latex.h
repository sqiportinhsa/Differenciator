#ifndef LATEX_GENERATING
#define LATEX_GENERATING

#include "../common.h"
#include "../Tree/tree.h"
#include "../Replace/replacer.h"

void set_tex_output(FILE *output);

FILE* get_output_stream();

void print_to_latex(const char *message, ...);

void init_latex(const char *filename);

void close_latex();

void latex_print_expr(Tree_node *head);

void latex_print_differenciation (const Transformation *transfs, int size);

void latex_print_simplifying     (const Transformation *transfs, int size);

void latex_print_replacing(const Tree_node *node, int number);

void latex_print_phrase();

void print_introduction(Tree *tree);

#endif
