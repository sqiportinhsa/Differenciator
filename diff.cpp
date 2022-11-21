#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "diff.h"
#include "Libs/file_reading.hpp"

#define memory_allocate(ptr, size, type, returning)                                           \
        ptr = (type*) calloc(size, sizeof(type));                                             \
        if (ptr == nullptr) {                                                                 \
            tree_dtor(tree);                                                                  \
            return returning;                                                                 \
        }

bool init_formula(Formula *formula, char *text_or) {
    
    assert(formula != nullptr);

    formula->text_origin = text_or;

    init_tree(formula->origin);
    init_tree(formula->diffirenciated);
}

bool parse_oper(Tree_node *node, char *text_origin) {
    size_t ip = 0;

    int bracket_balance = 0;

    if (text_origin != "(") {
        printf("Incorrect input format: '(' expected, '%c' got. Stop.\n", text_origin[ip]);
        return false;
    }

    ++ip;
    ++bracket_balance;

    if (isalpha(text_origin[ip])) {
        node->type = VAR;
        node->data.var = &(text_origin[ip]);
        return true;
    }

    if (isdigit(text_origin[ip])) {
        node->type = VAL;
        sscanf(&(text_origin[ip]), "%d", node->data.val);
    }

    if (text_origin[ip] != '(') {
        printf("Incorrect input format: bracket, alpha or digit expected."
               "Got <%c>. Stop.\n", text_origin[ip]);

        return false;
    }

    ++ip;
    ++bracket_balance;

    while (bracket_balance != 1) {
        if (text_origin[ip] == '(') {
            ++bracket_balance;
        }

        if (text_origin[ip] == ')') {
            --bracket_balance;
        }

        ++ip;
    }

    if (!get_oper(&text_origin[ip], &node->data.op)) {
        return false;
    }
}

bool get_oper(char *source, Operations *dest) {
    switch (*source) {
        case '+':
            *dest = ;
            return true;

        case '-':
            *dest = ;
            return true;
                    
        case '*':
            *dest = ;
            return true;
            
        case '/':
            *dest = ;
            return true;
            
        case '^':
            *dest = ;
            return true;
        
        default:
            printf("Incorrect operation");
                    
    }
}