#ifndef _EXPRESSIONSC_
#define _EXPRESSIONSC_

#include <stdlib.h>
#include <string.h>
#include "../tokens.c"

typedef struct expr expr;

typedef enum {
    INT_LIT, FLOAT_LIT, DOUBLE_LIT, CHAR_LIT, IDENT_LIT, NULL_LIT,
} literal_type;

typedef union {
    int i;
    float f;
    double d;
    char c;
    token t;
} literal_data;

typedef struct {
    literal_type type;
    literal_data data;
} literal;

typedef enum {
    INFIX_EXPR, PREFIX_EXPR, LITERAL_EXPR
} expr_type;

struct infix_expr {
    token operator;
    expr* left;
    expr* right;
};

struct prefix_expr {
    token operator;
    expr* right;
};

typedef union {
    struct infix_expr inf;
    struct prefix_expr pre;
    literal lit;
} expr_data;

typedef struct expr {
    expr_type type;
    expr_data data;
} expr;


// TODO
char* expression_string(expr* e) {
    char* expr_str = malloc(128);
    strcpy(expr_str, "");
    return expr_str;
}

#endif