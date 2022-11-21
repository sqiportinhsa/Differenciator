#ifndef TREE_H
#define TREE_H

#include "../Libs/logging.h"


static const char *FILL__COLOR = "#9AA5BB";
static const char *FRAME_COLOR = "#232D42";
static const char *ARROW_COLOR = "#54303c";

static const char *Data_is_val = "value";
static const char *Data_is_var = "variable";
static const char *Data_is_op  = "operator";

struct Tree_node {
    Node_type  type     = VAL;
    Tree_node* right    = nullptr;
    Tree_node* left     = nullptr;
    Tree_node* parent   = nullptr;

    union Data {
        double     val;
        Operations  op;
        char*      var;
    } data;

    bool is_saved = true;
};

struct Tree {
    Tree_node*       head      = nullptr;
    Creation_logs*   logs      = nullptr;
};

enum Tree_err {
    NO_TREE_ERR = 0,
    NOT_ENOUGHT_MEM = 1,
    CANNOT_GENER_PIC = 2,
};

enum Node_type {
    VAR,
    VAL,
    OP,
};

enum Operations {
    ADD,
    SUB,
    MUL,
    DIV,
    COS,
    SIN,
    TAN,
    CTG,
    LOG,
    DEG,
};



#define init_tree(tree) real_tree_init(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__);

#define dump_tree(tree, message, ...) real_dump_tree(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__,\
                                                                           message, ##__VA_ARGS__);

#define Print_code(format, ...)                    \
        fprintf(code_output, format, ##__VA_ARGS__);


#define Print_val_node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | val: %f}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                                     node, Data_is_val, node->data.val, FILL__COLOR, FRAME_COLOR);

#define Print_var_node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | var: %s}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                                     node, Data_is_var, node->data.var, FILL__COLOR, FRAME_COLOR);

#define Print_op__node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | op:  %d}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                                     node, Data_is_op,  node->data.op,  FILL__COLOR, FRAME_COLOR);


#define Print_arrow(node)                                                              \
        Print_code("node%p->node%p [color=\"%s\"];\n", node->parent, node, ARROW_COLOR);


int real_tree_init(Tree* tree, const char *file, const char *func, int line);

Tree_node* init_right_node(Tree *tree, Tree_node *parent, char *data);
Tree_node* init_left_node (Tree *tree, Tree_node *parent, char *data);
 
int init_head_node(Tree *tree, char *data);

void free_node(Tree_node *node);


void tree_dtor(Tree *tree);

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...);

void generate_graph_picture(const Tree *tree, char *picture_name);

void text_database_dump(Tree *tree, FILE *output);

void generate_file_name(char *filename, const char *extension);

#endif
