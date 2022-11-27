#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "tree.h"
#include "../Libs/file_reading.hpp"


static void generate_node_code(Tree_node *node, FILE *code_output);

static Tree_node* create_node(Tree_node *parent, bool is_left, 
                              Tree_node *left = nullptr, Tree_node *right = nullptr);

static void text_dump_node(Tree_node *node, FILE *output);


static const int max_file_with_graphviz_code_name_len = 30;
static const int max_generation_png_command_len = 200;
static const int max_png_file_name_len = 30;


//----------------------------- INITIALISATION SECTION -------------------------------------------//

#define memory_allocate(ptr, size, type, returning)                                           \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            printf("can't allocate memory: not enought free mem\n");                          \
            return returning;                                                                 \
        }


int real_tree_init(Tree* tree, const char *file, const char *func, int line) {
    memory_allocate(tree->logs, 1, Creation_logs, NOT_ENOUGHT_MEM);

    init_cr_logs(tree->logs, file, func, line);

    return NO_TREE_ERR;
}

//------------------- CREATION ----------------//

static Tree_node* create_node(Tree_node *parent, bool is_left, Tree_node *left, Tree_node *right) {

    Tree_node *node = nullptr;

    memory_allocate(node, 1, Tree_node, nullptr);

    node->left  = left;
    node->right = right;

    node->parent = parent;

    if (parent == nullptr) {

        return node;

    } else if (is_left) {

        parent->left  = node;

    } else {

        parent->right = node;
    } 

    return node;
}

Tree_node* create_right_node(Tree_node *parent, Tree_node *left, Tree_node *right) {
    return create_node(parent, false, left, right);
}

Tree_node* create_left_node (Tree_node *parent, Tree_node *left, Tree_node *right) {
    return create_node(parent, true,  left, right);
}

Tree_node* create_head_node(Tree_node *left, Tree_node *right) {
    return create_node(nullptr, false, left, right);
}

//------------- FILLING WITH DATA -------------//

Tree_node* fill_node(Node_type type, char var, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_node(parent, is_left, left, right);

    node->type = type;

    node->data.var = var;

    return node;
}

Tree_node* fill_node(Node_type type, int val, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_node(parent, is_left, left, right);

    node->type = type;

    node->data.val = val;

    return node;
}

Tree_node* fill_node(Node_type type, Operations op, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_node(parent, is_left, left, right);

    node->type = type;

    node->data.op = op;

    return node;
}


//--------------------------------- DESTRUCTION SECTION ------------------------------------------//

#define FREE_EXISTED(node)       \
        if (node != nullptr) {   \
            free_node(node);     \
        }

void tree_dtor(Tree *tree) {
    assert(tree != nullptr);

    free(tree->logs);
    
    free_node(tree->head);

    tree->head = nullptr;
    tree->logs = nullptr;
}

void free_node(Tree_node *node) {
    if (node == nullptr) {
        return;
    }

    FREE_EXISTED(node->left);
    FREE_EXISTED(node->right);

    free(node);
}

#undef FREE_EXISTED

//-------------------------------------- DUMP SECTION --------------------------------------------//

void real_dump_tree(const Tree *tree, const char *file, const char *func, int line, 
                                                               const char *message, ...) {
    
    FILE *output = GetLogStream();

    fprintf(output, "<b>Tree dump called in %s(%d), function %s: ", file, line, func);

    va_list ptr = {};
    va_start(ptr, message);
    vfprintf(output, message, ptr);
    va_end(ptr);

    fprintf(output, "\n</b>");
    
    if (tree == nullptr) {
        fprintf(output, "Can't dump tree from nullptr pointer\n");
        return;
    }

    fprintf(output, "Tree [%p] ", tree);

    if (tree->logs == nullptr) {
        fprintf(output, "without creation info (logs ptr in nullptr):\n");
    } else {
        fprintf(output, "created at %s(%d), function %s:\n", tree->logs->file_of_creation, 
                                                             tree->logs->line_of_creation, 
                                                             tree->logs->func_of_creation);
    }

    fflush(output);

    if (tree->head == nullptr) {
        fprintf(output, "\tCan't print data: tree root does not exist\n");
    } else {
        fprintf(output, "\tTree data visualisation:\n");

        char png_file_name[max_png_file_name_len] = {};

        generate_file_name(png_file_name, "png");

        generate_graph_picture(tree, png_file_name);

        #ifdef LOGS_TO_HTML
        fprintf(GetLogStream(), "\n<img src=\"%s\">\n", png_file_name);
        #else
        fprintf(GetLogStream(), "Picture is generated. You can find it by name %s.\n", png_file_name);
        #endif
    }

    fprintf(output, "\n");
    fprintf(output, "\n<hr>\n");
    fflush(output);

}

void generate_graph_picture(const Tree *tree, char *picture_name) {
    assert(tree         != nullptr);
    assert(picture_name != nullptr);

    char code_filename[max_file_with_graphviz_code_name_len] = {};
    generate_file_name(code_filename, "dot");

    FILE *code_output = fopen(code_filename, "w");

    Print_code("digraph G{\n");
    Print_code("node [shape=record,style=\"filled\"];\n");
    Print_code("splines=ortho;\n");

    generate_node_code(tree->head, code_output);

    Print_code("}");

    fclose(code_output);

    char command[max_generation_png_command_len] = {};

    sprintf(command, "dot %s -o %s -T png", code_filename, picture_name);

    system(command);
}

void text_database_dump(Tree *tree, FILE *output) {
    assert(tree   != nullptr);
    assert(output != nullptr);

    text_dump_node(tree->head, output);
}

static void text_dump_node(Tree_node *node, FILE *output) {
    assert(node   != nullptr);
    assert(output != nullptr);

    fprintf(output, "{ \"%s\"", node->data);

    if (node->left != nullptr && node->right != nullptr) {
        fprintf(output, "\n");
        text_dump_node(node->left,  output);
        text_dump_node(node->right, output);
    }

    fprintf(output, " }\n");
}

static void generate_node_code(Tree_node *node, FILE *code_output) {

    switch (node->type) {
        case VAL:
            Print_val_node(node);
            break;
        case VAR:
            Print_var_node(node);
            break;        
        case OP:
            Print_op__node(node);
            break;
        default:
            break;
    }
    
    if (node->parent) {
        Print_arrow(node);
    }

    if (node->left) {
        generate_node_code(node->left, code_output);
    }

    if (node->right) {
        generate_node_code(node->right, code_output);
    }
}

void generate_file_name(char *filename, const char *extension)  {
    static int file_with_graphviz_code_counter = 0;
    sprintf(filename, "Graphs/graph_%d.%s", file_with_graphviz_code_counter, extension);
    ++file_with_graphviz_code_counter;
}
