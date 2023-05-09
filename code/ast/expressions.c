#ifndef _EXPRESSIONSC_
#define _EXPRESSIONSC_

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../tokens.c"
#include "../parser/statements.c"

typedef struct expr expr;

typedef enum {
    INT_LIT, BOOL_LIT, IDENT_LIT, FN_LIT, NULL_LIT,
} literal_type;


typedef struct token_list {
    token* tokens;
    size_t count;
    size_t capacity;
} token_list;

token_list new_token_list() {
    token_list new_list;
    new_list.capacity = 1;
    new_list.tokens = (token*)malloc(new_list.capacity * sizeof(token));
    new_list.count = 0;
    return new_list;
}

void append_token_list(token_list* cur_list, token new_token) {
    if ((cur_list->count) >= cur_list->capacity) {
        cur_list->tokens = (token*)realloc(cur_list->tokens, cur_list->capacity * 2 * sizeof(token));
        cur_list->capacity *= 2;
    }
    cur_list->tokens[cur_list->count] = new_token;
    cur_list->count++;
}

char* token_list_string(token_list* tl) {
    char* expr_str;
    char* tl_str = malloc(128);
    strcpy(tl_str, "");

    for(int i = 0; i < tl->count; i++) {
        if (i == tl->count - 1) {
            sprintf(tl_str, "%s%s", tl_str, tl->tokens[i].value);
        } else {
            sprintf(tl_str, "%s%s, ", tl_str, tl->tokens[i].value);
        }
    }

    return tl_str;
}

struct fn_lit {
    token_list params;
    stmt_list body;
};

typedef union {
    int i;
    bool b;
    token t;
    struct fn_lit fn;
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
    bool has_alt;
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
        case FN_LIT:
            sprintf(lit_str, "fn(%s) { %s }", token_list_string(&lit.data.fn.params), program_string(&lit.data.fn.body));
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

    if (ifelse.has_alt) {
        sprintf(if_str, "if (%s) { %s } else { %s }", expression_string(ifelse.condition), program_string(&ifelse.consequence), program_string(&ifelse.alternative));
    }
    else {
        sprintf(if_str, "if (%s) { %s }", expression_string(ifelse.condition), program_string(&ifelse.consequence));
    }

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
