#ifndef _EXPRESSIONSC_
#define _EXPRESSIONSC_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../tokens.c"
#include "../parser/statements.c"

typedef struct expr expr;

typedef enum {
    INT_LIT, BOOL_LIT, IDENT_LIT, NULL_LIT,
} literal_type;

typedef union {
    int i;
    bool b;
    token t;
} literal_data;

typedef struct {
    literal_type type;
    literal_data data;
} literal;

typedef enum {
    INFIX_EXPR, PREFIX_EXPR, LITERAL_EXPR, IF_EXPR
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

struct if_expr {
    expr* condition;
    stmt_list consequence;
    stmt_list alternative;
};

typedef union {
    struct infix_expr inf;
    struct prefix_expr pre;
    struct if_expr ifelse;
    literal lit;
} expr_data;

typedef struct expr {
    expr_type type;
    expr_data data;
} expr;


char* expression_string(expr*);


char* literal_string(literal lit) {
    char* lit_str = malloc(128);

    switch (lit.type) {
        case IDENT_LIT:
            strcpy(lit_str, lit.data.t.value);
            break;
        case INT_LIT:
            sprintf(lit_str, "%d", lit.data.i);
            break;
        case BOOL_LIT:
            sprintf(lit_str, "%s", lit.data.b ? "true" : "false");
            break;
        default:
            strcpy(lit_str, "");
    }

    return lit_str;
}


char* prefix_string(struct prefix_expr pre) {
    char* pre_str = malloc(128);

    sprintf(pre_str, "%s%s", pre.operator.value, expression_string(pre.right));

    return pre_str;
}


char* infix_string(struct infix_expr inf) {
    char* inf_str = malloc(128);

    sprintf(inf_str, "(%s %s %s)", expression_string(inf.left), inf.operator.value, expression_string(inf.right));

    return inf_str;
}


char* if_string(struct if_expr ifelse) {
    char* if_str = malloc(128);

    sprintf(if_str, "if (%s) {%s} else {%s}", expression_string(ifelse.condition), program_string(&ifelse.consequence), program_string(&ifelse.alternative));

    return if_str;
}


char* expression_string(expr* e) {
    char* expr_str;

    switch (e->type) {
        case LITERAL_EXPR:
            return literal_string(e->data.lit);
        case PREFIX_EXPR:
            return prefix_string(e->data.pre);
        case INFIX_EXPR:
            return infix_string(e->data.inf);
        case IF_EXPR:
            return if_string(e->data.ifelse);
        default:
            expr_str = malloc(128);
            strcpy(expr_str, "");
            return expr_str;
    }
}

#endif
