#include <stdlib.h>
#include <string.h>
#include "../tokens.c"

typedef enum statement_type {
    SMT_LET, SMT_RETURN, SMT_EXPRESSION
} statement_type;

typedef struct statement {
    statement_type type;
    void* data;
    struct statement* next;
} statement;

typedef enum expression_type {
    EXPR_OPERATOR, EXPR_IDENT
} expression_type;

typedef struct expression {
    expression_type type;
    void* data;
} expression;

typedef struct program {
    statement* statements;
    int num_statements;
} program;

// Statement data types
typedef struct smt_let_data {
    // LET token
    token t;
    // Var name thats being set
    token ident;
    // Value the var is being set to
    expression value;
} smt_let_data;

typedef struct smt_return_data {
    // RETURN token
    token t;
    // Expression being returned
    expression value;
} smt_return_data;

typedef struct smt_expr_data {
    // Expression
    expression value;
} smt_expr_data;

// Expression data types
//
char* expression_string(expression* e) {
    char* expr_str = malloc(128);
    strcpy(expr_str, "");
    return expr_str;
}

char* statement_string(statement* s) {
    smt_let_data* let_data;
    smt_return_data* return_data;
    smt_expr_data* expr_data;

    char* expr_str;
    char* stmt_str = malloc(128);

    switch (s->type) {
        case SMT_LET:
            let_data = (smt_let_data *)s->data;
            expr_str = expression_string(&let_data->value);
            sprintf(stmt_str, "%s %s = %s;\n", let_data->t.value, let_data->ident.value, expr_str);
            free(expr_str);
            break;
        case SMT_RETURN:
            return_data = (smt_return_data *)s->data;
            expr_str = expression_string(&return_data->value);
            sprintf(stmt_str, "%s %s;\n", return_data->t.value, expr_str);
            free(expr_str);
            break;
        case SMT_EXPRESSION:
            expr_data = (smt_expr_data *)s->data;
            expr_str = expression_string(&return_data->value);
            sprintf(stmt_str, "%s;\n", expr_str);
            free(expr_str);
            break;
    }

    return stmt_str;
}

char* program_string(program* p) {
    char* stmt_str = malloc(2048);
    char* cur_str; 
    int str_pos = 0;
    statement* cur_stmt = p->statements;

    // Just tack together all the statement strings
    for (int i = 0; i < p->num_statements; i++) {
        cur_str = statement_string(cur_stmt);
        strcpy(stmt_str + str_pos, cur_str);
        str_pos += strlen(cur_str);
        cur_stmt = cur_stmt->next;
    }
    
    return stmt_str;
}
