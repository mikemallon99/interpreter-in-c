#ifndef _EXPRESSIONSH_
#define _EXPRESSIONSH_

#include <stdbool.h>
#include <stdlib.h>
// #include <crtdbg.h>

#include "../tokens.h"
#include "../parser/statements.h"

typedef enum
{
    INT_LIT,
    STRING_LIT,
    BOOL_LIT,
    IDENT_LIT,
    FN_LIT,
    NULL_LIT,
} literal_type;

typedef struct expr expr;
typedef struct environment environment;

typedef struct token_list
{
    token* tokens;
    size_t count;
    size_t capacity;
} token_list;

struct fn_lit
{
    token_list params;
    stmt_list body;
    environment* env;
};

typedef union
{
    int i;
    bool b;
    token t;
    char* s;
    struct fn_lit fn;
} literal_data;

typedef struct
{
    literal_type type;
    literal_data data;
} literal;

typedef enum
{
    INFIX_EXPR,
    PREFIX_EXPR,
    LITERAL_EXPR,
    IF_EXPR,
    CALL_EXPR
} expr_type;

struct infix_expr
{
    token op;
    expr* left;
    expr* right;
};

struct prefix_expr
{
    token op;
    expr* right;
};

struct if_expr
{
    expr* condition;
    stmt_list consequence;
    bool has_alt;
    stmt_list alternative;
};

typedef struct expr_list
{
    expr** exprs;
    size_t count;
    size_t capacity;
} expr_list;

struct call_expr
{
    expr* func;
    expr_list args;
};

typedef union
{
    struct infix_expr inf;
    struct prefix_expr pre;
    struct if_expr ifelse;
    struct call_expr call;
    literal lit;
} expr_data;

typedef struct expr
{
    expr_type type;
    expr_data data;
} expr;

token_list new_token_list();
void append_token_list(token_list* cur_list, token new_token);

expr_list new_expr_list();
void append_expr_list(expr_list* cur_list, expr* new_expr);

char* token_list_string(token_list* tl);
char* expr_list_string(expr_list* el);
char* literal_string(literal lit);
char* lit_type_string(literal lit);
char* prefix_string(struct prefix_expr pre);
char* infix_string(struct infix_expr inf);
char* if_string(struct if_expr ifelse);
char* call_string(struct call_expr call);
char* expression_string(expr* e);

#endif
