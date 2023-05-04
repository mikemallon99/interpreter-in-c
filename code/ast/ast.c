#ifndef _ASTC_
#define _ASTC_

#include <stdlib.h>
#include "../tokens.c"
#include "../parser/parser.c"
#include "expressions.c"

typedef enum {
    LOWEST_PR, EQUALS_PR, LESSGREATER_PR, SUM_PR, PRODUCT_PR, PREFIX_PR, CALL_PR
} precedence;

precedence get_precedence(token t) {
    switch (t.type) {
        case EQ:
            return EQUALS_PR;
        case NOT_EQ:
            return EQUALS_PR;
        case LT:
            return LESSGREATER_PR;
        case GT:
            return LESSGREATER_PR;
        case PLUS:
            return SUM_PR;
        case MINUS:
            return SUM_PR;
        case SLASH:
            return PRODUCT_PR;
        case ASTERISK:
            return PRODUCT_PR;
        default:
            return LOWEST_PR;
    }
}


expr* parse_literal(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    literal lit;

    switch (p->cur_token.type) {
        case IDENT:
            lit.type = IDENT_LIT;
            lit.data.t = p->cur_token;
            break;
        case INT:
            lit.type = INT_LIT;
            lit.data.i = atoi(p->cur_token.value);
            break;
        default:
            lit.type = NULL_LIT;
            break;
    }

    ex->data.lit = lit;
    return ex;
}


expr* parse_prefix(parser* p) {
    switch (p->cur_token.type) {
        // Literals
        case IDENT:
        case INT:
            return parse_literal(p);
        default:
            return NULL;
    }
}


expr* parse_expression(precedence prec) {
    
}

#endif
