#ifndef _ASTC_
#define _ASTC_

#include <stdlib.h>
#include "../tokens.c"
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
        case LPAREN:
            return CALL_PR;
        default:
            return LOWEST_PR;
    }
}


#endif
