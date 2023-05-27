#ifndef _ASTH_
#define _ASTH_

#include "../tokens.h"

typedef enum {
    LOWEST_PR, EQUALS_PR, LESSGREATER_PR, SUM_PR, PRODUCT_PR, PREFIX_PR, CALL_PR
} precedence;

precedence get_precedence(token t);

#endif