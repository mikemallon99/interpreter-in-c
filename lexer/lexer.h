#ifndef _LEXERH_
#define _LEXERH_

#include <stdbool.h>

#include "../tokens.h"

typedef struct {
    char* input_string;
    int position;
    int read_position;
    char cur_byte;
} lexer;

char peek_char(lexer* l);
void read_char(lexer* l);
lexer get_lexer(char* input_string);
bool is_number(char cur_byte);
bool is_letter(char cur_byte);
char* read_identifier(lexer* l);
char* read_int(lexer* l);
void skip_whitespace(lexer* l);
token next_lexer_token(lexer* l);

#endif
