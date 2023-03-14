#include <string.h>
#include "../tokens.c"

void test_next_token() {
    char input[] = "=+(){},;";
    token assign;
    assign.tokenType = EOF_T;
    assign.value = 0x00;

    token tests[] = {
        {.tokenType = ASSIGN, .value = '='},
        {.tokenType = PLUS, .value = '+'},
        {.tokenType = LPAREN, .value = '('},
        {.tokenType = RPAREN, .value = ')'},
        {.tokenType = LBRACE, .value = '{'},
        {.tokenType = RBRACE, .value = '}'},
        {.tokenType = COMMA, .value = ','},
        {.tokenType = SEMICOLON, .value = ';'},
    };
}
