#ifndef TREE_H
#define TREE_H

#include "../Libs/logging.h"


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
    DEG = 5,
    ADD = 10,
    SUB = 11,
    MUL = 12,
    DIV = 13,
    COS = 30,
    SIN = 31,
    TAN = 32,
    CTG = 33,
    LOG = 34,
    EXP = 35,
};

struct Tree_node {
    Node_type  type     = VAL;
    Tree_node* right    = nullptr;
    Tree_node* left     = nullptr;
    Tree_node* parent   = nullptr;

    union Data {
        int        val;
        Operations  op;
        char       var;
    } data;

    int  weight  = 0;
    int  replace = 0;
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


Tree_node* create_node(char var, 
                      Tree_node *left   = nullptr, 
                      Tree_node *right  = nullptr);

Tree_node* create_node(int val, 
                      Tree_node *left   = nullptr, 
                      Tree_node *right  = nullptr);


Tree_node* create_node(Operations op, 
                      Tree_node *left   = nullptr, 
                      Tree_node *right  = nullptr);


Tree_node* copy_subtree(Tree_node *source);


void set_as_parent(Tree_node *node);

int  count_nodes(const Tree_node *head);


void free_node(Tree_node *node);

void tree_dtor(Tree *tree);


#define dump_tree(tree, message, ...) real_dump_tree(tree, __FILE__, __PRETTY_FUNCTION__, __LINE__,\
                                                                           message, ##__VA_ARGS__);

#define dump_subtree(tree, message, ...) real_dump_subtree(tree, __FILE__, __PRETTY_FUNCTION__, \
                                                              __LINE__, message, ##__VA_ARGS__);

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...);

void real_dump_subtree(const Tree_node *head, const char *file, const char *func, int line, 
                                                                       const char *message, ...);


void generate_graph_picture(const Tree *tree, char *picture_name);

void generate_file_name(char *filename, const char *extension);

#endif
