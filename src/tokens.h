#ifndef _TOKENSH_
#define _TOKENSH_

typedef enum
{
    ILLEGAL,
    EOF_T,

    // Identifiers & Literals
    IDENT,
    STRING,
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
    COLON,
    SEMICOLON,
    DB_QUOTE,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,

    // Keywords
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN
} token_type;

typedef struct
{
    token_type type;
    char* value;
} token;

token_type lookup_ident(char* ident);
char* get_token_type_string(token_type t);

#endif
