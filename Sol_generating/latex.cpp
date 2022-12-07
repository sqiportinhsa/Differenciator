#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>

#include "latex.h"

#include "../phrases.h"

const char *latex_filename = "latex_output.tex";

FILE *tex_output = nullptr;

static void print_head();


static void latex_print_node         (const Tree_node *node);

static void latex_print_add_and_sub  (const Tree_node *node);

static void latex_print_transc       (const Tree_node *node);

static void latex_print_div          (const Tree_node *node);
        
static void latex_print_mul          (const Tree_node *node);
        
static void latex_print_deg          (const Tree_node *node);

static bool priority_lower_than_mul  (const Tree_node *node);


//-------------------------- OUTPUT FUNCTIONS SECTION --------------------------------------------//

void set_tex_output(FILE *output) {

    tex_output = output;
}

FILE* get_output_stream() {

    return tex_output;
}

void print_to_latex(const char *message, ...) {

    va_list ptr = {};
    va_start(ptr, message);
    vfprintf(get_output_stream(), message, ptr);
    va_end(ptr);

}

void latex_print_phrase() {
    print_to_latex("%s\n", Phrases[rand() % Phrases_num]);
}

//------------------------ PRINTING COMMON -------------------------------------------------------//


void init_latex(const char *filename) {

    FILE *output = fopen(latex_filename, "w");

    set_tex_output(output);

    print_head();

}

void close_latex() {

    print_to_latex("\\end{document}");

    fclose(get_output_stream());
}

static void print_head() {

    print_to_latex("\\documentclass[12pt,a4paper,fleqn]{article}\n"
                   "\\usepackage[utf8]{inputenc}\n"
                   "\\usepackage{amssymb, amsmath, multicol}\n"
                   "\\usepackage[russian]{babel}\n"
                   "\\usepackage{graphicx}\n"
                   "\\usepackage[shortcuts,cyremdash]{extdash}\n"
                   "\\usepackage{wrapfig}\n"
                   "\\usepackage{floatflt}\n"
                   "\\usepackage{lipsum}\n"
                   "\\usepackage{euler}\n"
                   "\n"
                   "\\oddsidemargin=0mm\n"
                   "\\textwidth=160mm\n"
                   "\\headheight=-32.4mm\n"
                   "\\textheight=277mm\n"
                   "\\tolerance=100\n"
                   "\\parindent=5pt\n"
                   "\\parskip=8pt\n"
                   "\\pagestyle{empty}\n"
                   "\\usepackage[normalem]{ulem}\n"
                   "\\usepackage{mdframed}\n"
                   "\\usepackage{amsthm}\n");

    print_to_latex("\\begin{document}\n"
                   "\\begin{center}\n"
                   "\\Large\\textbf{Пособие по математическому анализу.}\n"
                   "\n"
                   "\\large\\textbf{Дифферинцирование на практике с подробным решением.}\n"
                   "\\end{center}\n\n");

}

void print_diff_introduction(Tree *tree) {
    print_to_latex("\\section{Вступление}"
                   "Добрый день, дорогие друзья. \\sout{Вы продулись и откровенно неправильно "
                   "решили регион...} Вы хорошо проводили время весь семестр и пришло время сдавать"
                   " задание по матану? Если у вас в голове пусто, и вы не знаете, как решать "
                   "задачи или просто хотите проверить себя \\sout{потому что писали код "
                   "всю ночь и сомневаетесь в своей адекватности}, то я здесь, чтобы помочь вам. "
                   "Электронный дифференциатор \\sout{ошибается похлеще вас, вспомните, когда"
                   "он был написан} в отличие от человека не допускает ошибки и способен понятно "
                   "объяснить решение с первых дней жизни, в чем вам предстоит убедиться при "
                   "прочтении этого файла. На повестке дня следующее выражение:\n\n");

    latex_print_expr(tree->head->left);
}

//--------------------------------- PRINTING EXPRESSIONS -----------------------------------------//


void latex_print_expr(Tree_node *head) {

    print_to_latex("$$");

    latex_print_node(head);

    print_to_latex("$$\n");

}

void latex_print_diff(const Tree_node *orig, const Tree_node *diff) {

    latex_print_phrase();

    print_to_latex("$$\\frac{d}{dx}(");

    latex_print_node(orig);

    print_to_latex(") = ");

    latex_print_node(diff);

    print_to_latex("$$\n");
}

void latex_print_simplify(const Tree_node *orig, const Tree_node *simpl) {

    latex_print_phrase();

    print_to_latex("$$");

    latex_print_node(orig);

    print_to_latex(" = ");

    latex_print_node(simpl);

    print_to_latex("$$\n");
}

static void latex_print_node(const Tree_node *node) {

    if (node->type == VAL) {

        if (node->data.val < 0) {
            print_to_latex("(%d)", node->data.val);

        } else {
            print_to_latex("%d", node->data.val);
        }

        return;
    }

    if (node->type == VAR) {

        print_to_latex("%c", node->data.var);

        return;
    } 

    switch (node->data.op) {
        case ADD: case SUB: 
            latex_print_add_and_sub(node);
            break;

        case SIN: case COS: case TAN: case CTG: case EXP: case LOG:
            latex_print_transc(node);
            break;

        case DIV:
            latex_print_div(node);
            break;

        case MUL:
            latex_print_mul(node);
            break;

        case DEG:
            latex_print_deg(node);
            break;

        default:
            fprintf(stderr, "Add operation %d to %s\n", node->data.op, __PRETTY_FUNCTION__);
            break;
    }
}

static void latex_print_add_and_sub(const Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == ADD || node->data.op == SUB);

    latex_print_node(node->left);

    switch (node->data.op) {
        case ADD:
            print_to_latex("+");
            break;
        case SUB:
            print_to_latex("-");
            break;
        default:
            break;
    }

    latex_print_node(node->right);
}

static void latex_print_transc(const Tree_node *node) {

    assert(node != nullptr);

    switch (node->data.op) {
        case SIN:
            print_to_latex("\\sin(");
            break;
        case COS:
            print_to_latex("\\cos(");
            break;
        case TAN:
            print_to_latex("\\tan(");
            break;
        case CTG:
            print_to_latex("\\ctg(");
            break;
        case EXP:
            print_to_latex("\\exp(");
            break;
        case LOG:
            print_to_latex("\\ln(");
            break;
        default:
            break;
    }

    latex_print_node(node->left);

    print_to_latex(")");
}

static void latex_print_div(const Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == DIV);

    print_to_latex("\\frac{");
    latex_print_node(node->left);
    print_to_latex("}{");
    latex_print_node(node->right);
    print_to_latex("}");
}

#define PRINT_NODE_WITH_BRACKETS(node)               \
        if (priority_lower_than_mul(node)) {         \
        print_to_latex("(");                         \
        latex_print_node(node);                      \
        print_to_latex(")");                         \
                                                     \
    } else {                                         \
        latex_print_node(node);                      \
    }

static void latex_print_mul(const Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.val == MUL);

    PRINT_NODE_WITH_BRACKETS(node->left);

    print_to_latex("*");
    
    PRINT_NODE_WITH_BRACKETS(node->right);

}

#undef PRINT_NODE_WITH_BRACKETS

static void latex_print_deg(const Tree_node *node) {

    assert(node != nullptr);
    assert(node->data.op == DEG);

    print_to_latex("{");
    latex_print_node(node->left);
    print_to_latex("}^{");
    latex_print_node(node->right);
    print_to_latex("}");
}

static bool priority_lower_than_mul(const Tree_node *node) {

    assert(node != nullptr);

    if (node->type != OP) {

        return false;
    }

    if (node->data.op == ADD || node->data.op == SUB) {

        return true;
    }

    return false;
}
