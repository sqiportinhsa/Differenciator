#ifndef TREE_H
#define TREE_H

#include "../Libs/logging.h"


static const char *FILL__COLOR = "#9AA5BB";
static const char *FRAME_COLOR = "#232D42";
static const char *ARROW_COLOR = "#54303c";

static const char *Data_is_val = "value";
static const char *Data_is_var = "variable";
static const char *Data_is_op  = "operator";


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
    EXP,
};

struct Tree_node {
    Node_type  type     = VAL;
    Tree_node* right    = nullptr;
    Tree_node* left     = nullptr;
    Tree_node* parent   = nullptr;

    union Data {
        double     val;
        Operations  op;
        char      var;
    } data;

};

struct Tree {
    Tree_node*       head      = nullptr;
    Creation_logs*   logs      = nullptr;
};



#define init_tree(tree) real_tree_init(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__);


int real_tree_init(Tree* tree, const char *file, const char *func, int line);

Tree_node* create_empty_node(Tree_node *parent = nullptr, Tree_node *left  = nullptr, 
                                                    Tree_node *right = nullptr);

Tree_node* create_orphan_node(Tree_node *left = nullptr, Tree_node *right = nullptr);


Tree_node* create_node(Node_type type, char var, 
                     Tree_node *left   = nullptr, 
                     Tree_node *right  = nullptr);

Tree_node* create_node(Node_type type, int val, 
                     Tree_node *left   = nullptr, 
                     Tree_node *right  = nullptr);


Tree_node* create_node(Node_type type, Operations op, 
                     Tree_node *left   = nullptr, 
                     Tree_node *right  = nullptr);


void set_as_parent(Tree_node *node);


void free_node(Tree_node *node);

void tree_dtor(Tree *tree);


#define dump_tree(tree, message, ...) real_dump_tree(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__,\
                                                                           message, ##__VA_ARGS__);

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...);

void generate_graph_picture(const Tree *tree, char *picture_name);

void text_database_dump(Tree *tree, FILE *output);

void generate_file_name(char *filename, const char *extension);

#endif
