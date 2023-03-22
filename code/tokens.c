#include <stdio.h>
#include <stdlib.h>
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

char* get_token_type_string(token_type t) {
    char* token_string = calloc(10, 1);

    switch (t) {
        case ILLEGAL:
            strcpy(token_string, "ILLEGAL");
            break;
        case EOF_T:
            strcpy(token_string, "EOF_T");
            break;
        case IDENT:
            strcpy(token_string, "IDENT");
            break;
        case INT:
            strcpy(token_string, "INT");
            break;
        case ASSIGN:
            strcpy(token_string, "ASSIGN");
            break;
        case PLUS:
            strcpy(token_string, "PLUS");
            break;
        case MINUS:
            strcpy(token_string, "MINUS");
            break;
        case BANG:
            strcpy(token_string, "BANG");
            break;
        case ASTERISK:
            strcpy(token_string, "ASTERISK");
            break;
        case SLASH:
            strcpy(token_string, "SLASH");
            break;
        case LT:
            strcpy(token_string, "LT");
            break;
        case GT:
            strcpy(token_string, "GT");
            break;
        case EQ:
            strcpy(token_string, "EQ");
            break;
        case NOT_EQ:
            strcpy(token_string, "NOT_EQ");
            break;
        case COMMA:
            strcpy(token_string, "COMMA");
            break;
        case SEMICOLON:
            strcpy(token_string, "SEMICOLON");
            break;
        case LPAREN:
            strcpy(token_string, "LPAREN");
            break;
        case RPAREN:
            strcpy(token_string, "RPAREN");
            break;
        case LBRACE:
            strcpy(token_string, "LBRACE");
            break;
        case RBRACE:
            strcpy(token_string, "RBRACE");
            break;
        case FUNCTION:
            strcpy(token_string, "FUNCTION");
            break;
        case LET:
            strcpy(token_string, "LET");
            break;
        case TRUE:
            strcpy(token_string, "TRUE");
            break;
        case FALSE:
            strcpy(token_string, "FALSE");
            break;
        case IF:
            strcpy(token_string, "IF");
            break;
        case ELSE:
            strcpy(token_string, "ELSE");
            break;
        case RETURN:
            strcpy(token_string, "RETURN");
            break;
    }
    return token_string;
}
