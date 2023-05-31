#include <stdlib.h>
#include <crtdbg.h>

#include "ast.h"

precedence get_precedence(token t)
{
    switch (t.type)
    {
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
