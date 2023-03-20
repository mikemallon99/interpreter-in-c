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
    MINUS,
    BANG,
    ASTERISK, 
    SLASH,
    
    LT,
    GT,
    EQ,
    NOT_EQ,

    // Delimiters
    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN
} token_type;

typedef struct Tokens {
    token_type tokenType;
    char* value;
} token;

token_type lookup_ident(char* ident) {
    if (strcmp(ident, "fn") == 0) {
        return FUNCTION;
    } else if (strcmp(ident, "let") == 0) {
        return LET;
    } else if (strcmp(ident, "true") == 0) {
        return TRUE;
    } else if (strcmp(ident, "false") == 0) {
        return FALSE;
    } else if (strcmp(ident, "if") == 0) {
        return IF;
    } else if (strcmp(ident, "else") == 0) {
        return ELSE;
    } else if (strcmp(ident, "return") == 0) {
        return RETURN;
    } else {
        return IDENT;
    }
}
