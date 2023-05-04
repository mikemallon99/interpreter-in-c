#ifndef _EXPRESSIONSC_
#define _EXPRESSIONSC_

typedef enum expression_type {
    EXPR_OPERATOR, EXPR_IDENT
} expression_type;

typedef struct expression {
    expression_type type;
    void* data;
} expression;

#endif
