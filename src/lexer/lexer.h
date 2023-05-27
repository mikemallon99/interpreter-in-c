#ifndef _LEXERH_
#define _LEXERH_

#include <stdbool.h>

#include "../tokens.h"

typedef struct
{
    char* input_string;
    int position;
    int read_position;
    char cur_byte;
} lexer;

lexer get_lexer(char* input_string);
token next_lexer_token(lexer* l);

#endif
