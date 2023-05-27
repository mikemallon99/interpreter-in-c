#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lexer.h"

char _peek_char(lexer *l);

void _read_char(lexer *l);
char *_read_int(lexer *l);
char *_read_identifier(lexer *l);

bool _is_number(char cur_byte);
bool _is_letter(char cur_byte);
void _skip_whitespace(lexer *l);

// PUBLIC FUNCTIONS

lexer get_lexer(char *input_string)
{
    lexer l;
    l.input_string = input_string;
    l.read_position = 0;
    l.position = 0;
    _read_char(&l);
    return l;
}

token next_lexer_token(lexer *l)
{
    token t;
    char *char_str;

    _skip_whitespace(l);

    // Read token
    switch (l->cur_byte)
    {
    case '=':
        if (_peek_char(l) == '=')
        {
            char_str = calloc(3, 1);
            char_str[0] = l->cur_byte;
            _read_char(l);
            char_str[1] = l->cur_byte;
            t = (token){EQ, char_str};
        }
        else
        {
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){ASSIGN, char_str};
        }
        break;
    case '+':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){PLUS, char_str};
        break;
    case '-':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){MINUS, char_str};
        break;
    case '!':
        if (_peek_char(l) == '=')
        {
            char_str = calloc(3, 1);
            char_str[0] = l->cur_byte;
            _read_char(l);
            char_str[1] = l->cur_byte;
            t = (token){NOT_EQ, char_str};
        }
        else
        {
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){BANG, char_str};
        }
        break;
    case '*':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){ASTERISK, char_str};
        break;
    case '/':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){SLASH, char_str};
        break;
    case '<':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){LT, char_str};
        break;
    case '>':
        char_str = calloc(2, 1);
        char_str[0] = l->cur_byte;
        t = (token){GT, char_str};
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
        if (_is_letter(l->cur_byte))
        {
            // Return early since we dont want to advance the next char
            char *identifier = _read_identifier(l);
            return (token){lookup_ident(identifier), identifier};
        }
        else if (_is_number(l->cur_byte))
        {
            // Return early since we dont want to advance the next char
            char *identifier = _read_int(l);
            return (token){INT, identifier};
        }
        else
        {
            char_str = calloc(2, 1);
            char_str[0] = l->cur_byte;
            t = (token){ILLEGAL, char_str};
        }
    }

    _read_char(l);
    return t;
}

// PRIVATE FUNCTIONS

char _peek_char(lexer *l)
{
    if (l->read_position >= strlen(l->input_string))
    {
        return 0;
    }
    else
    {
        return l->input_string[l->read_position];
    }
}

void _read_char(lexer *l)
{
    if (l->read_position >= strlen(l->input_string))
    {
        l->cur_byte = 0;
    }
    else
    {
        l->cur_byte = l->input_string[l->read_position];
    }
    l->position = l->read_position;
    l->read_position += 1;
}

bool _is_number(char cur_byte)
{
    return (cur_byte >= '0' && cur_byte <= '9');
}

bool _is_letter(char cur_byte)
{
    return ((cur_byte >= 'a' && cur_byte <= 'z') || (cur_byte >= 'A' && cur_byte <= 'Z') || cur_byte == '_');
}

char *_read_identifier(lexer *l)
{
    int position = l->position;
    while (_is_letter(l->cur_byte) || _is_number(l->cur_byte))
    {
        _read_char(l);
    }
    // Need to allocate memory for new char array here
    int id_len = l->position - position;
    char *identifier = (char *)calloc(id_len + 1, 1);
    strncpy(identifier, l->input_string + position, id_len);
    return identifier;
}

char *_read_int(lexer *l)
{
    int position = l->position;
    while (_is_number(l->cur_byte))
    {
        _read_char(l);
    }
    // Need to allocate memory for new char array here
    int id_len = l->position - position;
    char *identifier = (char *)calloc(id_len, 1);
    strncpy(identifier, l->input_string + position, id_len);
    return identifier;
}

void _skip_whitespace(lexer *l)
{
    while (l->cur_byte == ' ' || l->cur_byte == '\n')
    {
        _read_char(l);
    }
}
