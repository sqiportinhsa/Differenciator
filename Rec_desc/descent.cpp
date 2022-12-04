#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "descent.h"

#include "../Tree/tree.h"

const int max_name_len = 3;

#define DEBUG

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                   DECLARATIONS SECTION                                         //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

// GETTING ARGUMENT

static Tree_node* get_argument      (const char **pointer);

static Tree_node* get_value         (const char **pointer);

static Tree_node* get_variable      (const char **pointer);

static Tree_node* get_from_brackets (const char **pointer);


// GETTING FUNCTION

static Tree_node* get_transc        (const char **pointer);
  
static Tree_node* get_add_and_sub   (const char **pointer);
  
static Tree_node* get_mul_and_div   (const char **pointer);

static Tree_node* get_deg           (const char **pointer);


// GRAMMAR
//
//  Descent           ::= Get_add_and_sub, '\0';
//  Get_add_and_sub   ::= Get_mul_and_div {[+, -]}*
//  Get_mul_and_div   ::= Get_deg {[*, /]}*
//  Get_deg           ::= Get_transc {[^]}*
//  Get_transc        ::= [sin/cos/...] '(' Get_add_and_sub ')' | Get_from_brackets
//  Get_from_brackets ::= '(' Get_add_sub ')' | Get_argument
//  Get_argument      ::= Get_value | Get_variable
//  Get_value         ::= ['0' - '9']+
//  Get_variable      ::= ['a' - 'z', 'A' - 'Z']
//


#ifdef DEBUG

#define DEBUG_PRINT(str, ...) {\
    printf ("Func: %20s  line %3d " str, __func__, __LINE__, ##__VA_ARGS__);\
}

#else 

#define DEBUG_PRINT(str, ...) ;

#endif


//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                          MAIN PARSING FUNCTION AND COMMON DEFINES                              //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define WARNING(checkup, message, ...)                     \
        if (!(checkup)) {                                  \
            printf("Error: " message, ##__VA_ARGS__);      \
            return nullptr;                                \
        }

#define RETURN_NULLPTR_IF(condition) \
        if (condition)               \
            return nullptr;


Tree_node* descent(const char *pointer) {

    assert(pointer != nullptr);

    DEBUG_PRINT("start\n");

    Tree_node *head = get_add_and_sub(&pointer);

    WARNING(*pointer == '\0', "unexpected symbol: <%c>\n", *pointer);

    return head;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                              GETTING ARGUMENT (value/variblde)                                 //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define TRY_TO_GET_WITH(got)         \
        node = got;                  \
        if (node != nullptr) {       \
            return node;             \
        }


static Tree_node* get_argument(const char **pointer) {
    
    assert(pointer != nullptr);

    Tree_node *node = nullptr;
    
    TRY_TO_GET_WITH(get_value   (pointer));
    TRY_TO_GET_WITH(get_variable(pointer));

    WARNING(true, "invalid syntax. Value or variable expected, got <%c>", *pointer);

    return nullptr;
}

static Tree_node* get_value(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    int val = 0;

    const char *pointer_before = *pointer;

    while (isdigit(**pointer)) {

        val = (val * 10) + **pointer - '0';

        ++(*pointer);
    }

    DEBUG_PRINT("got %d\n", val);

    if (pointer_before == *pointer) {
        return nullptr;
    }

    return create_node(VAL, val);

}

static Tree_node* get_variable(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    if (isalpha(*(*pointer + 0)) && !isalpha(*(*pointer + 1))) {

        Tree_node *node = create_node(VAR, **pointer);

        ++(*pointer);

        DEBUG_PRINT("got %c\n", node->data.var);
        
        return node;
    }
    
    return nullptr;
}

static Tree_node* get_from_brackets(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    Tree_node *node = nullptr;

    if (**pointer == '(') {

        ++(*pointer);

        node = get_add_and_sub(pointer);

        WARNING(**pointer == ')', "close bracket ')' expected\n");

        ++(*pointer);

    } else {

        node = get_argument(pointer);
    }

    return node;
}

#undef TRY_TO_GET_WITH

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                           GETTING FUNCTIONS (transcedent, arithmetic)                          //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define CHECK_IF_FUNC_IS(name, func_type)                   \
        if (strncasecmp(func, name, strlen(name)) == 0) {   \
            node->type = OP;                                \
            node->data.op = func_type;                      \
            *pointer += strlen(name);                       \
            func_parsed = true;                             \
        }


static Tree_node* get_transc(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    char func[max_name_len] = {};

    sscanf(*pointer, "%c%c%c", &func[0], &func[1], &func[2]);

    bool func_parsed = false;

    Tree_node *node = create_empty_node();

    CHECK_IF_FUNC_IS("sin", SIN);
    CHECK_IF_FUNC_IS("cos", COS);
    CHECK_IF_FUNC_IS("tan", TAN);
    CHECK_IF_FUNC_IS("ctg", CTG);
    CHECK_IF_FUNC_IS("log", LOG);
    CHECK_IF_FUNC_IS("exp", EXP);

    if (!func_parsed) {
        return get_from_brackets(pointer);
    }

    RETURN_NULLPTR_IF(**pointer != '(');

    ++(*pointer);
    
    node->left = get_add_and_sub(pointer);

    set_as_parent(node);

    RETURN_NULLPTR_IF(**pointer != ')');

    ++(*pointer);

    return node;
}

static Tree_node* get_add_and_sub(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    DEBUG_PRINT("start get add and sub\n");

    Tree_node *node = get_mul_and_div(pointer);

    RETURN_NULLPTR_IF(node == nullptr);

    DEBUG_PRINT("got %d\n", node->data.val);

    while (**pointer == '+' || **pointer == '-') {

        DEBUG_PRINT("cont get add and sub\n");

        Operations op = ADD;

        if (**pointer == '+') {       //get type of operation
            op = ADD;
        } else {
            op = SUB;
        }

        ++(*pointer);

        node = create_node(OP, op, node, get_mul_and_div(pointer));

        RETURN_NULLPTR_IF(node        == nullptr);
        RETURN_NULLPTR_IF(node->right == nullptr);
    }

    return node;
}

static Tree_node* get_mul_and_div(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    
    DEBUG_PRINT("start get mul and div\n");

    Tree_node *node = get_deg(pointer);

    RETURN_NULLPTR_IF(node == nullptr);

    DEBUG_PRINT("got %d\n", node->data.val);

    while (**pointer == '*' || **pointer == '/') {

        DEBUG_PRINT("cont get mul and div\n");

        Operations op = MUL;

        if (**pointer == '*') {       //get type of operation
            op = MUL;
        } else {
            op = DIV;
        }

        ++(*pointer);

        node = create_node(OP, op, node, get_deg(pointer));

        RETURN_NULLPTR_IF(node        == nullptr);
        RETURN_NULLPTR_IF(node->right == nullptr);
    }

    return node;
}

static Tree_node* get_deg(const char **pointer) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);

    DEBUG_PRINT("start get deg\n");

    Tree_node *node = get_transc(pointer);

    RETURN_NULLPTR_IF(node == nullptr);

    DEBUG_PRINT("got %d\n", node->data.val);

    while (**pointer == '^') {

        DEBUG_PRINT("cont get deg\n");

        ++(*pointer);

        node = create_node(OP, DEG, node, get_transc(pointer));

        RETURN_NULLPTR_IF(node        == nullptr);
        RETURN_NULLPTR_IF(node->right == nullptr);
    }

    return node;
}

#undef CHECK_IF_FUNC_IS

#undef WARNING
#undef RETURN_FALSE_IF
