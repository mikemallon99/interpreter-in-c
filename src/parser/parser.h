#ifndef _PARSERH_
#define _PARSERH_

#include "../lexer/lexer.h"
#include "../eval/eval.h"
#include "statements.h"
#include "../ast/ast.h"

typedef struct
{
    lexer *l;
    token cur_token;
    token peek_token;
    // This needs to be a list of strings
    char errors[64][256];
    int num_errors;
} parser;

parser new_parser(lexer *l);
stmt_list parse_program(parser *p);
bool check_parser_errors(parser *p);

#endif