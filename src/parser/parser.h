#ifndef _PARSERH_
#define _PARSERH_

#include "../lexer/lexer.h"
#include "../eval/eval.h"
#include "statements.h"
#include "../ast/ast.h"

typedef struct {
    lexer* l;
    token cur_token;
    token peek_token;
    // This needs to be a list of strings
    char errors[64][256];
    int num_errors;
} parser; 

void next_parser_token(parser* p);
void peek_error(parser* p, token_type cur_type);
bool cur_token_is(parser* p, token_type cur_type);
bool peek_token_is(parser* p, token_type peek_type);
precedence peek_precedence(parser* p);
bool expect_peek(parser* p, token_type peek_type);
bool check_parser_errors(parser* p);
parser new_parser(lexer* l);
expr* parse_literal(parser* p);
expr* parse_prefix(parser* p);
expr* parse_infix(parser* p, expr* left);
expr* parse_group(parser* p);
stmt_list parse_block_stmt(parser* p);
expr* parse_if(parser* p);
token_list parse_fn_params(parser* p);
expr_list parse_call_args(parser* p);
expr* parse_call(parser* p, expr* left);
expr* parse_fn(parser* p);
expr* parse_expression(parser* p, precedence prec);
stmt parse_let_statement(parser* p);
stmt parse_return_statement(parser* p);
stmt parse_expr_stmt(parser* p);
stmt parse_statement(parser* p);
stmt_list parse_program(parser* p);

#endif