#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "tree.h"
#include "../Libs/file_reading.hpp"


static void generate_node_code(const Tree_node *node, FILE *code_output);


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

Tree_node* create_empty_node(Tree_node *parent, Tree_node *left, Tree_node *right) {

    Tree_node *node = nullptr;

    memory_allocate(node, 1, Tree_node, nullptr);

    node->left  = left;
    node->right = right;

    node->parent = parent;

    node->weight  = 0;
    node->replace = 0;

    if (node->left) {
        node->left->parent = node;
    }

    if (node->right) {
        node->right->parent = node;
    }

    return node;
}

Tree_node* create_orphan_node(Tree_node *left, Tree_node *right) {
    return create_empty_node(nullptr, left, right);
}


Tree_node* create_node(char var, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_orphan_node(left, right);

    node->type = VAR;

    node->data.var = var;

    return node;
}

Tree_node* create_node(int val, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_orphan_node(left, right);

    node->type = VAL;

    node->data.val = val;

    return node;
}

Tree_node* create_node(Operations op, Tree_node *left, Tree_node *right) {

    Tree_node *node = create_orphan_node(left, right);

    node->type = OP;

    node->data.op = op;

    return node;
}

Tree_node* copy_subtree(Tree_node *source) {

    Tree_node *copy = create_empty_node();

    copy->data = source->data;
    copy->type = source->type;

    if (source->left) {

        copy->left  = copy_subtree(source->left);
    }

    if (source->right) {

        copy->right = copy_subtree(source->right);
    }

    return copy;
}

//----------------------------------- USEFUL FUNCTIONS -------------------------------------------//

void set_as_parent(Tree_node *node) {

    if (node->left) {

        node->left->parent  = node;
    }

    if (node->right) {

        node->right->parent = node;
    }
}

int count_nodes(const Tree_node *head) {

    assert(head != nullptr);

    int number = 1;

    if (head->left) {

        number += count_nodes(head->left);
    }

    if (head->right) {

        number += count_nodes(head->right);
    }

    return number;
}


//--------------------------------- DESTRUCTION SECTION ------------------------------------------//

#define FREE_EXISTED(node)       \
        if (node != nullptr) {   \
            free_node(node);     \
        }

void tree_dtor(Tree *tree) {
    assert(tree != nullptr);

    dump_tree(tree, "tree before death");

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

#define Print_code(format, ...)                    \
        fprintf(code_output, format, ##__VA_ARGS__);

#define Print_val_node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | val: %d | rep: %d}\",fillcolor=\"%s\",color=\"%s\"];\n",    \
                        node, Data_is_val, node->data.val, node->replace, FILL__COLOR, FRAME_COLOR);

#define Print_var_node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | var: %c | rep: %d}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                        node, Data_is_var, node->data.var, node->replace, FILL__COLOR, FRAME_COLOR);

#define Print_op__node(node)                                                                       \
        Print_code("node%p [label=\"{type: %s | op:  %d | rep: %d}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                        node, Data_is_op,  node->data.op, node->replace,  FILL__COLOR, FRAME_COLOR);

#define Print_head_node(node)\
        Print_code("node%p [label=\"{head node}\",fillcolor=\"%s\",color=\"%s\"];\n",     \
                                                     node, FILL__COLOR, FRAME_COLOR);


#define Print_parent_arrow(node)                                                       \
        Print_code("node%p->node%p [color=\"%s\"];\n", node, node->parent, ARROW_COLOR);

#define Print_right_child_arrow(node)                                                 \
        Print_code("node%p->node%p [color=\"%s\"];\n", node, node->right, ARROW_COLOR);

#define Print_left_child_arrow(node)                                                 \
        Print_code("node%p->node%p [color=\"%s\"];\n", node, node->left, ARROW_COLOR);


#ifdef DEBUG

#define DEBUG_PRINT(str, ...) {\
    printf ("Func: %20s  line %3d " str, __func__, __LINE__, ##__VA_ARGS__);\
}

#else 

#define DEBUG_PRINT ;

#endif


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

void real_dump_subtree(const Tree_node *head, const char *file, const char *func, int line, 
                                                               const char *message, ...) {
    
    FILE *output = GetLogStream();

    fprintf(output, "Subree dump called in %s(%d), function %s: ", file, line, func);

    va_list ptr = {};
    va_start(ptr, message);
    vfprintf(output, message, ptr);
    va_end(ptr);

    fprintf(output, "\n</b>");
    
    if (head == nullptr) {
        fprintf(output, "Can't dump tree from nullptr pointer\n");
        return;
    }

    fprintf(output, "Subtree head [%p] with head data %d\n", head, head->data.val);

    char png_file_name[max_png_file_name_len] = {};

    generate_file_name(png_file_name, "png");

    char code_filename[max_file_with_graphviz_code_name_len] = {};
    generate_file_name(code_filename, "dot");

    FILE *code_output = fopen(code_filename, "w");

    DEBUG_PRINT("%s\n", code_filename);

    Print_code("digraph G{\n");
    Print_code("node [shape=record,style=\"filled\"];\n");
    Print_code("splines=ortho;\n");

    generate_node_code(head, code_output);

    Print_code("}");

    fclose(code_output);

    char command[max_generation_png_command_len] = {};

    sprintf(command, "dot %s -o %s -T png", code_filename, png_file_name);

    system(command);

    fprintf(GetLogStream(), "Picture is generated. You can find it by name %s.\n", png_file_name);

    fprintf(output, "\n");

    fflush(output);
}

void generate_graph_picture(const Tree *tree, char *picture_name) {
    assert(tree         != nullptr);
    assert(picture_name != nullptr);

    char code_filename[max_file_with_graphviz_code_name_len] = {};
    generate_file_name(code_filename, "dot");

    FILE *code_output = fopen(code_filename, "w");

    DEBUG_PRINT("%s\n", code_filename);

    Print_code("digraph G{\n");
    Print_code("node [shape=record,style=\"filled\"];\n");
    Print_code("splines=ortho;\n");
    
    DEBUG_PRINT("given head is %d", tree->head->data.val);

    Print_head_node(tree->head);

    generate_node_code(tree->head->left, code_output); //skip fictive head

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

    fprintf(output, "{ \"%c\"", node->data);

    if (node->left != nullptr && node->right != nullptr) {
        fprintf(output, "\n");
        text_dump_node(node->left,  output);
        text_dump_node(node->right, output);
    }

    fprintf(output, " }\n");
}

static void generate_node_code(const Tree_node *node, FILE *code_output) {

    DEBUG_PRINT("start dumping node %d.\n", node->data.val);

    switch (node->type) {
        case VAL:
            DEBUG_PRINT("node type is val. \n", node->data.val);
            Print_val_node(node);
            break;
        case VAR:
            Print_var_node(node);
            break;        
        case OP:
            DEBUG_PRINT("node type is op. \n", node->data.op);
            Print_op__node(node);
            break;
        default:
            break;
    }
    
    if (node->parent) {
        Print_parent_arrow(node);
        DEBUG_PRINT("its parent is %d\n", node->parent->data.val);
    }

    if (node->left) {
        Print_left_child_arrow(node);
        DEBUG_PRINT("node %d left child is %d\n", node->data.val, node->left->data.val);
        generate_node_code(node->left, code_output);
    }

    if (node->right) {
        Print_right_child_arrow(node);
        DEBUG_PRINT("node %d right child is %d\n", node->data.val, node->right->data.val);
        generate_node_code(node->right, code_output);
    }
}

void generate_file_name(char *filename, const char *extension)  {
    static int file_with_graphviz_code_counter = 0;
    sprintf(filename, "Graphs/graph_%d.%s", file_with_graphviz_code_counter, extension);
    ++file_with_graphviz_code_counter;
}
