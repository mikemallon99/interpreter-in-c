#include "../tokens.c"

typedef enum statement_type {
    SMT_LET
} statement_type;

typedef struct statement {
    statement_type type;
    void* data;
    struct statement* next;
} statement;

typedef enum expression_type {
    EXPR_OPERATOR
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

// Expression data types
