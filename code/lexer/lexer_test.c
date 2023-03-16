#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "lexer.c"

void test_simple() {
    char input[] = "=+(){},;";

    token tests[] = {
        {.tokenType = ASSIGN, .value = "="},
        {.tokenType = PLUS, .value = "+"},
        {.tokenType = LPAREN, .value = "("},
        {.tokenType = RPAREN, .value = ")"},
        {.tokenType = LBRACE, .value = "{"},
        {.tokenType = RBRACE, .value = "}"},
        {.tokenType = COMMA, .value = ","},
        {.tokenType = SEMICOLON, .value = ";"},
    };
    
    lexer l = get_lexer(input);

    token cur_token;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        printf("Char #%d\n", i);
        cur_token = next_token(&l);
        if (cur_token.tokenType != tests[i].tokenType) {
            printf("%d != %d\n", cur_token.tokenType, tests[i].tokenType);
        }
        if (strcmp(cur_token.value, tests[i].value) != 0) {
            printf("%s != %s\n", cur_token.value, tests[i].value);
        } else {
            printf("%s == %s\n", cur_token.value, tests[i].value);
        }
    }
}

void test_code() {
    char* input = 
        "let five = 5;\n"
        "let ten = 10;\n"
        "let add = fn(x, y) {\n"
        "   x + y;\n"
        "};\n"
        "\n"
        "let result = add(five, ten);\n";

    token tests[] = {
        {.tokenType = LET, .value = "let"},
        {.tokenType = IDENT, .value = "five"},
        {.tokenType = ASSIGN, .value = "="},
        {.tokenType = INT, .value = "5"},
        {.tokenType = SEMICOLON, .value = ";"},
        {.tokenType = LET, .value = "let"},
        {.tokenType = IDENT, .value = "ten"},
        {.tokenType = ASSIGN, .value = "="},
        {.tokenType = INT, .value = "10"},
        {.tokenType = SEMICOLON, .value = ";"},
        {.tokenType = LET, .value = "let"},
        {.tokenType = IDENT, .value = "add"},
        {.tokenType = ASSIGN, .value = "="},
        {.tokenType = FUNCTION, .value = "fn"},
        {.tokenType = LPAREN, .value = "("},
        {.tokenType = IDENT, .value = "x"},
        {.tokenType = COMMA, .value = ","},
        {.tokenType = IDENT, .value = "y"},
        {.tokenType = RPAREN, .value = ")"},
        {.tokenType = LBRACE, .value = "{"},
        {.tokenType = IDENT, .value = "x"},
        {.tokenType = PLUS, .value = "+"},
        {.tokenType = IDENT, .value = "y"},
        {.tokenType = SEMICOLON, .value = ";"},
        {.tokenType = RBRACE, .value = "}"},
        {.tokenType = SEMICOLON, .value = ";"},
        {.tokenType = LET, .value = "let"},
        {.tokenType = IDENT, .value = "result"},
        {.tokenType = ASSIGN, .value = "="},
        {.tokenType = IDENT, .value = "add"},
        {.tokenType = LPAREN, .value = "("},
        {.tokenType = IDENT, .value = "five"},
        {.tokenType = COMMA, .value = ","},
        {.tokenType = IDENT, .value = "ten"},
        {.tokenType = RPAREN, .value = ")"},
        {.tokenType = SEMICOLON, .value = ";"},
        {.tokenType = EOF_T, .value = "\0"},

    };
    
    lexer l = get_lexer(input);

    token cur_token;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        printf("Char #%d\n", i);
        cur_token = next_token(&l);
        if (cur_token.tokenType != tests[i].tokenType) {
            printf("%d != %d\n", cur_token.tokenType, tests[i].tokenType);
        }
        if (strcmp(cur_token.value, tests[i].value) != 0) {
            printf("%s != %s\n", cur_token.value, tests[i].value);
        } else {
            printf("%s == %s\n", cur_token.value, tests[i].value);
        }
    }
}

int main() {
    printf("Test Simple:\n");
    test_simple();

    printf("Test Code:\n");
    test_code();
    return 0;
}
