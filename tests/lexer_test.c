#ifndef _LEXERTESTC_
#define _LEXERTESTC_

#include <string.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "../src/lexer/lexer.h"

void test_simple()
{
    char input[] = "=+(){},;";

    token tests[] = {
        {.type = ASSIGN, .value = "="},
        {.type = PLUS, .value = "+"},
        {.type = LPAREN, .value = "("},
        {.type = RPAREN, .value = ")"},
        {.type = LBRACE, .value = "{"},
        {.type = RBRACE, .value = "}"},
        {.type = COMMA, .value = ","},
        {.type = SEMICOLON, .value = ";"},
    };

    lexer l = get_lexer(input);

    bool passed = true;
    token cur_token;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        cur_token = next_lexer_token(&l);
        if (cur_token.type != tests[i].type)
        {
            printf("Char #%d\n", i);
            printf("%d != %d\n", cur_token.type, tests[i].type);
            passed = false;
        }
        if (strcmp(cur_token.value, tests[i].value) != 0)
        {
            printf("Char #%d\n", i);
            printf("%s != %s\n", cur_token.value, tests[i].value);
            passed = false;
        }
        // else {
        //     printf("%s == %s\n", cur_token.value, tests[i].value);
        // }
        free(cur_token.value);
    }
    if (passed)
    {
        printf("Passed!\n");
    }
}

void test_code()
{
    char* input =
        "let five = 5;\n"
        "let ten = 10;\n"
        "let add = fn(x, y) {\n"
        "   x + y;\n"
        "};\n"
        "\n"
        "let result = add(five, ten);\n"
        "!-/*5;\n"
        "5 < 10 > 5;\n"
        "\n"
        "if (5 < 10) {\n"
        "    return true;\n"
        "} else {\n"
        "    return false;\n"
        "}\n"
        "\n"
        "10 == 10;\n"
        "10 != 9;\n";

    token tests[] = {
        {.type = LET, .value = "let"},
        {.type = IDENT, .value = "five"},
        {.type = ASSIGN, .value = "="},
        {.type = INT, .value = "5"},
        {.type = SEMICOLON, .value = ";"},
        {.type = LET, .value = "let"},
        {.type = IDENT, .value = "ten"},
        {.type = ASSIGN, .value = "="},
        {.type = INT, .value = "10"},
        {.type = SEMICOLON, .value = ";"},
        {.type = LET, .value = "let"},
        {.type = IDENT, .value = "add"},
        {.type = ASSIGN, .value = "="},
        {.type = FUNCTION, .value = "fn"},
        {.type = LPAREN, .value = "("},
        {.type = IDENT, .value = "x"},
        {.type = COMMA, .value = ","},
        {.type = IDENT, .value = "y"},
        {.type = RPAREN, .value = ")"},
        {.type = LBRACE, .value = "{"},
        {.type = IDENT, .value = "x"},
        {.type = PLUS, .value = "+"},
        {.type = IDENT, .value = "y"},
        {.type = SEMICOLON, .value = ";"},
        {.type = RBRACE, .value = "}"},
        {.type = SEMICOLON, .value = ";"},
        {.type = LET, .value = "let"},
        {.type = IDENT, .value = "result"},
        {.type = ASSIGN, .value = "="},
        {.type = IDENT, .value = "add"},
        {.type = LPAREN, .value = "("},
        {.type = IDENT, .value = "five"},
        {.type = COMMA, .value = ","},
        {.type = IDENT, .value = "ten"},
        {.type = RPAREN, .value = ")"},
        {.type = SEMICOLON, .value = ";"},
        {.type = BANG, .value = "!"},
        {.type = MINUS, .value = "-"},
        {.type = SLASH, .value = "/"},
        {.type = ASTERISK, .value = "*"},
        {.type = INT, .value = "5"},
        {.type = SEMICOLON, .value = ";"},
        {.type = INT, .value = "5"},
        {.type = LT, .value = "<"},
        {.type = INT, .value = "10"},
        {.type = GT, .value = ">"},
        {.type = INT, .value = "5"},
        {.type = SEMICOLON, .value = ";"},
        {.type = IF, .value = "if"},
        {.type = LPAREN, .value = "("},
        {.type = INT, .value = "5"},
        {.type = LT, .value = "<"},
        {.type = INT, .value = "10"},
        {.type = RPAREN, .value = ")"},
        {.type = LBRACE, .value = "{"},
        {.type = RETURN, .value = "return"},
        {.type = TRUE, .value = "true"},
        {.type = SEMICOLON, .value = ";"},
        {.type = RBRACE, .value = "}"},
        {.type = ELSE, .value = "else"},
        {.type = LBRACE, .value = "{"},
        {.type = RETURN, .value = "return"},
        {.type = FALSE, .value = "false"},
        {.type = SEMICOLON, .value = ";"},
        {.type = RBRACE, .value = "}"},
        {.type = INT, .value = "10"},
        {.type = EQ, .value = "=="},
        {.type = INT, .value = "10"},
        {.type = SEMICOLON, .value = ";"},
        {.type = INT, .value = "10"},
        {.type = NOT_EQ, .value = "!="},
        {.type = INT, .value = "9"},
        {.type = SEMICOLON, .value = ";"},
        {.type = EOF_T, .value = "\0"},

    };

    lexer l = get_lexer(input);

    token cur_token;
    bool passed = true;
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
    {
        cur_token = next_lexer_token(&l);
        if (cur_token.type != tests[i].type)
        {
            printf("token #%d\n", i);
            printf("%d != %d\n", cur_token.type, tests[i].type);
            passed = false;
        }
        if (strcmp(cur_token.value, tests[i].value) != 0)
        {
            printf("value #%d\n", i);
            printf("%s != %s\n", cur_token.value, tests[i].value);
            passed = false;
        }
        else
        {
            printf("%s == %s\n", cur_token.value, tests[i].value);
        }
        free(cur_token.value);
    }
    if (passed)
    {
        printf("Passed!\n");
    }
}

void run_all_lexer_tests()
{
    printf("Test Simple:\n");
    test_simple();

    printf("Test Code:\n");
    test_code();
}

#endif
