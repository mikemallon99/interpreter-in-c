#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../tokens.c"

typedef struct Lexers {
    char* input_string;
    int position;
    int read_position;
    char cur_byte;
} lexer;

void read_char(lexer* l) {
    if (l->read_position >= strlen(l->input_string)) {
        l->cur_byte = 0;
    } else {
        l->cur_byte = l->input_string[l->read_position];
    }
    l->position = l->read_position;
    l->read_position += 1;
}

lexer get_lexer(char* input_string) {
    lexer l;
    l.input_string = input_string;
    l.read_position = 0;
    l.position = 0;
    read_char(&l);
    return l;
}

bool is_number(char cur_byte) {
    if (cur_byte >= '0' && cur_byte <= '9') {
        return true;
    } else {
        return false;
    }
}

bool is_letter(char cur_byte) {
    if ((cur_byte >= 'a' && cur_byte <= 'z') || (cur_byte >= 'A' && cur_byte <= 'Z') || cur_byte == '_') {
        return true;
    } else {
        return false;
    }
}

char* read_identifier(lexer* l) {
    int position = l->position;
    while (is_letter(l->cur_byte) || is_number(l->cur_byte)) {
        read_char(l);
    }
    // Need to allocate memory for new char array here
    int id_len = l->position - position;
    char* identifier = (char*)calloc(id_len, 1);
    strncpy(identifier, l->input_string + position, id_len);
    return identifier;
}

char* read_int(lexer* l) {
    int position = l->position;
    while (is_number(l->cur_byte)) {
        read_char(l);
    }
    // Need to allocate memory for new char array here
    int id_len = l->position - position;
    char* identifier = (char*)calloc(id_len, 1);
    strncpy(identifier, l->input_string + position, id_len);
    return identifier;
}

void skip_whitespace(lexer* l) {
    while (l->cur_byte == ' ' || l->cur_byte == '\n') {
        read_char(l);
    }
}

token next_token(lexer* l) {
    token t;
    char* char_str;

    skip_whitespace(l);

    // Read token
    switch (l->cur_byte) {
        case '=':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){ASSIGN, char_str};
            break;
        case ';':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){SEMICOLON, char_str};
            break;
        case '(':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){LPAREN, char_str};
            break;
        case ')':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){RPAREN, char_str};
            break;
        case ',':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){COMMA, char_str};
            break;
        case '+':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){PLUS, char_str};
            break;
        case '{':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){LBRACE, char_str};
            break;
        case '}':
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){RBRACE, char_str};
            break;
        case 0:
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){EOF_T, char_str};
            break;
        default:
            if (is_letter(l->cur_byte)) {
                char* identifier = read_identifier(l);
                if (strcmp(identifier, "fn") == 0) {
                    t = (token){FUNCTION, identifier};
                }
                else if (strcmp(identifier, "let") == 0) {
                    t = (token){LET, identifier};
                }
                else {
                    t = (token){IDENT, identifier};
                }
                return t;
            } else if (is_number(l->cur_byte)) {
                char* identifier = read_int(l);
                t = (token){INT, identifier};
                return t;
            } else {
                char_str = calloc(2, 1);
                char_str[0] = l->cur_byte;
                t = (token){ILLEGAL, char_str};
            }
    }

    read_char(l);
    return t;
}
