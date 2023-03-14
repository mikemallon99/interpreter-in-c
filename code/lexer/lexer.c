#include <string.h>
#include "../tokens.c"

typedef struct Lexers {
    char* input_string;
    int position;
    int read_position;
    char cur_byte;
} lexer;

void read_char(lexer l) {
    if (l.read_position >= strlen(l.input_string)) {
        l.cur_byte = 0;
    } else {
        l.cur_byte = l.input_string[l.read_position];
    }
    l.position = l.read_position;
    l.read_position += 1;
}

lexer get_lexer(char* input_string) {
    lexer l;
    l.input_string = input_string;
    read_char(l);
    return l;
}

token next_token(lexer l) {
    token t;
    switch (l.cur_byte) {
        case '=':
            t.tokenType = ASSIGN;
            strcpy(t.value, (char[2]){l.cur_byte, '\0'});
        case ';':
            t = (token){SEMICOLON, {l.cur_byte, '\0'}};
        case '(':
            t = (token){LPAREN, {l.cur_byte, '\0'}};
        case ')':
            t = (token){RPAREN, {l.cur_byte, '\0'}};
        case ',':
            t = (token){COMMA, {l.cur_byte, '\0'}};
        case '+':
            t = (token){PLUS, {l.cur_byte, '\0'}};
        case '{':
            t = (token){LBRACE, {l.cur_byte, '\0'}};
        case '}':
            t = (token){RBRACE, {l.cur_byte, '\0'}};
        case 0:
            t = (token){EOF_T, "\0"};
    }

    read_char(l);
    return t;
}
