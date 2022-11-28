#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "descent.h"

#include "../Tree/tree.h"

const int max_name_len = 3;

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                                   DECLARATIONS SECTION                                         //
//                                                                                                //
//------------------------------------------------------------------------------------------------//

// GETTING ARGUMENT

static bool get_argument(const char **pointer, Tree_node *dest);

static bool get_value(const char **pointer, Tree_node *dest);

static bool get_variable(const char **pointer, Tree_node *dest);

static bool get_from_brackets(const char **pointer, Tree_node *dest);


// GETTING FUNCTION

static bool get_transc(const char **pointer, Tree_node *dest);

static bool get_add_sub(const char **pointer, Tree_node *dest);

static bool get_mul_and_div(const char **pointer, Tree_node *dest);

// GRAMMAR
//
//  Descent           ::= Get_add_sub, '\0';
//  Get_add_sub       ::= Get_mul_and_div {[+, -]}*
//  Get_mul_and_div   ::= Get_transc {[*, /]}*
//  Get_transc        ::= Get_from_brackets | [sin/cos/...] '(' Get_from_brackets ')'
//  Get_from_brackets ::= '(' Get_add_sub ')' | Get_argument
//  Get_argument      ::= Get_value | Get_variable
//  Get_value         ::= ['0' - '9']+
//  Get_variable      ::= ['a' - 'z', 'A' - 'Z']
//



//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                          MAIN PARSING FUNCTION AND COMMON DEFINES                              //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define WARNING(checkup, message, ...)                     \
        if (!(checkup)) {                                  \
            printf("Error: " message, ##__VA_ARGS__);      \
            return false;                                  \
        }

#define RETURN_FALSE_IF(statement) \
        if (statement)             \
            return false;


bool descent(const char *pointer, Tree_node *dest) {

    assert(pointer != nullptr);

    printf("start\n");

    RETURN_FALSE_IF(!get_add_sub(&pointer, dest));

    WARNING(*pointer == '\0', "unexpected symbol: <%c>\n", *pointer);

    return true;
}

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                              GETTING ARGUMENT (value/variblde)                                 //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define TRY_TO_GET_WITH(success)   \
        if (success) {             \
            return true;           \
        }


static bool get_argument(const char **pointer, Tree_node *dest) {
    
    assert(pointer != nullptr);
    
    TRY_TO_GET_WITH(get_value   (pointer, dest));
    TRY_TO_GET_WITH(get_variable(pointer, dest));

    WARNING(true, "invalid syntax. Value or variable expected, got <%c>", *pointer);

    return false;
}

static bool get_value(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    int val = 0;

    const char *pointer_before = *pointer;

    while (isdigit(**pointer)) {

        val = (val * 10) + **pointer - '0';

        ++(*pointer);
    }

    printf("got %d\n", val);

    if (pointer_before == *pointer) {
        return false;
    }

    dest->type = VAL;
    dest->data.val = val;

    return true;
}

static bool get_variable(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    if (isalpha(**pointer)) {

        dest->type = VAR;

        dest->data.var = **pointer;

        ++(*pointer);

        printf("got %c\n", dest->data.var);
        
        return true;
    }
    
    return false;
}

static bool get_from_brackets(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    if (**pointer == '(') {

        ++(*pointer);

        RETURN_FALSE_IF(!get_add_sub(pointer, dest));

        WARNING(**pointer == ')', "close bracket ')' expected\n");

        ++(*pointer);

    } else {

        RETURN_FALSE_IF(!get_argument(pointer, dest));
    }

    return true;
}

#undef TRY_TO_GET_WITH

//------------------------------------------------------------------------------------------------//
//                                                                                                //
//                           GETTING FUNCTIONS (transcedent, arithmetic)                          //
//                                                                                                //
//------------------------------------------------------------------------------------------------//


#define CHECK_IF_FUNC_IS(name, func_type)                   \
        if (strncasecmp(func, name, sizeof(name)) == 0) {   \
            dest->type = OP;                                \
            dest->data.op = func_type;                      \
            *pointer += sizeof(name);                       \
            func_parsed = true;                             \
        }


static bool get_transc(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    char func[max_name_len] = {};

    sscanf(*pointer, "%c%c%c", func[0], func[1], func[2]);

    if (get_from_brackets(pointer, dest)) {
        return true;
    }

    bool func_parsed = false;

    CHECK_IF_FUNC_IS("sin", SIN);
    CHECK_IF_FUNC_IS("cos", COS);
    CHECK_IF_FUNC_IS("tg",  TAN);
    CHECK_IF_FUNC_IS("ctg", CTG);
    CHECK_IF_FUNC_IS("log", LOG);
    CHECK_IF_FUNC_IS("exp", EXP);

    RETURN_FALSE_IF(!func_parsed);

    RETURN_FALSE_IF(**pointer != '(');

    ++(*pointer);
    
    dest->left = create_empty_node(dest);
    
    RETURN_FALSE_IF(!get_from_brackets(pointer, dest));

    RETURN_FALSE_IF(**pointer != ')');

    ++(*pointer);

    return true;

}

static bool get_add_sub(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    RETURN_FALSE_IF(!get_mul_and_div(pointer, dest));

    printf("cont get add anf sub\n");

    while (**pointer == '+' || **pointer == '-') {

        Tree_node temp = *dest;                          //save info from dest node

        printf("add before manipulations: %d\n", dest->data.val);

        dest->left = create_empty_node(dest);

        *(dest->left) = temp;                            //move saved info to left

        printf("add after moving to left: %d\n", dest->data.val);
                        
        dest->type = OP;                                 //make dest the operation node
                        
        if (**pointer == '+') {                          //get type of operation

            dest->data.op = ADD;

        } else {

            dest->data.op = SUB;

        }

        dest->right = create_empty_node(dest);           //create node for second operation argument
           
        dest = dest->right;                              //start work with second argument node

        printf("got %c\n", **pointer);

        ++(*pointer);

        RETURN_FALSE_IF(!get_mul_and_div(pointer, dest));//get second argument

    }

    return true;
}

static bool get_mul_and_div(const char **pointer, Tree_node *dest) {

    assert( pointer != nullptr);
    assert(*pointer != nullptr);
    assert( dest    != nullptr);

    RETURN_FALSE_IF(!get_transc(pointer, dest));

    printf("cont get mul and div\n");

    while (**pointer == '*' || **pointer == '/') {

        Tree_node temp = *dest;                          //save info from dest node

        dest->left = create_empty_node(dest);

        *(dest->left) = temp;                            //move saved info to left
                         
        dest->type = OP;                                 //make dest the operation node
                         
        if (**pointer == '*') {                          //get type of operation

            dest->data.op = MUL;

        } else {

            dest->data.op = DIV;

        }

        dest->right = create_empty_node(dest);           //create node for second operation argument

        dest = dest->right;                              //start work with second node

        printf("got %c\n", **pointer);

        ++(*pointer);

        RETURN_FALSE_IF(!get_transc(pointer, dest));     //get second argument

    }

    return true;
}

#undef CHECK_IF_FUNC_IS

#undef WARNING
#undef RETURN_FALSE_IF
