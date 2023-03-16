#include <stdio.h>
#include <string.h>

typedef enum token_type {
    ILLEGAL,
    EOF_T,

    // Identifiers & Literals
    IDENT,
    INT,

    // Operators
    ASSIGN,
    PLUS,

    // Delimiters
    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // Keywords
    FUNCTION,
    LET
} token_type;

typedef struct Tokens {
    token_type tokenType;
    char* value;
} token;

