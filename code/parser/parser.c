#ifndef _PARSERC_
#define _PARSERC_

#include "../lexer/lexer.c"
#include "statements.c"
#include "../ast/ast.c"
#include <stdlib.h>

typedef struct {
    lexer* l;
    token cur_token;
    token peek_token;
    // This needs to be a list of strings
    char errors[64][256];
    int num_errors;
} parser; 


void next_parser_token(parser* p) {
    p->cur_token = p->peek_token;
    p->peek_token = next_lexer_token(p->l);
}


void peek_error(parser* p, token_type cur_type) {
    sprintf(p->errors[p->num_errors], "Expected next token to be %s but got %s instead.", get_token_type_string(cur_type), get_token_type_string(p->peek_token.type));
    p->num_errors++;
}


bool cur_token_is(parser* p, token_type cur_type) {
    return p->cur_token.type == cur_type;
}


bool peek_token_is(parser* p, token_type peek_type) {
    return p->peek_token.type == peek_type;
}


bool expect_peek(parser* p, token_type peek_type) {
    if (peek_token_is(p, peek_type)) {
        next_parser_token(p);
        return true;
    } else {
        peek_error(p, peek_type);
        return false;
    }
}

bool check_parser_errors(parser* p) {
    if (p->num_errors == 0) return true;

    printf("Parser has %d errors:\n", p->num_errors);
    for (int i = 0; i < p->num_errors; i++) {
        printf("Error: %s\n", p->errors[i]);
    }
    return false;
}


parser new_parser(lexer* l) {
    parser p;
    p.l = l;
    p.num_errors = 0;

    // Call this twice to populate cur_token
    next_parser_token(&p);
    next_parser_token(&p);

    return p;
}


expr* parse_literal(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    literal lit;

    switch (p->cur_token.type) {
        case IDENT:
            lit.type = IDENT_LIT;
            lit.data.t = p->cur_token;
            break;
        case INT:
            lit.type = INT_LIT;
            lit.data.i = atoi(p->cur_token.value);
            break;
        default:
            lit.type = NULL_LIT;
            break;
    }

    ex->data.lit = lit;
    return ex;
}


expr* parse_prefix(parser* p) {
    switch (p->cur_token.type) {
        // Literals
        case IDENT:
        case INT:
            return parse_literal(p);
        default:
            return NULL;
    }
}


expr* parse_expression(parser* p, precedence prec) {
    expr* left_expr = parse_prefix(p);
    return left_expr;
}


stmt parse_let_statement(parser* p) {
    let_stmt let;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;
    
    // Verify next token is an identifier
    if (!expect_peek(p, IDENT)) {
        return new_stmt;
    }
    let.identifier = p->cur_token;

    // Verify next token is an equals sign
    if (!expect_peek(p, ASSIGN)) {
        return new_stmt;
    }

    next_parser_token(p);

    let.value = parse_expression(p, LOWEST_PR);

    if (peek_token_is(p, SEMICOLON)) {
        next_parser_token(p);
    }

    new_stmt.type = LET_STMT;
    new_stmt.data.let = let;
    return new_stmt;
}


stmt parse_return_statement(parser* p) {
    return_stmt ret;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;
    
    next_parser_token(p);

    ret.value = parse_expression(p, LOWEST_PR);

    if (peek_token_is(p, SEMICOLON)) {
        next_parser_token(p);
    }

    new_stmt.type = RETURN_STMT;
    new_stmt.data.ret = ret;
    return new_stmt;
}


stmt parse_expr_stmt(parser* p) {
    expr_stmt ex;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;
    
    ex.value = parse_expression(p, LOWEST_PR);

    if (peek_token_is(p, SEMICOLON)) {
        next_parser_token(p);
    }

    new_stmt.type = EXPR_STMT;
    new_stmt.data.expr = ex;
    return new_stmt;
}


stmt parse_statement(parser* p) {
    switch (p->cur_token.type) {
        case LET:
            return parse_let_statement(p);
        case RETURN:
            return parse_return_statement(p);
        default:
            return parse_expr_stmt(p);
    }
}


stmt_list parse_program(parser* p) {
    stmt_list prog = new_stmt_list();
    stmt cur_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    for (token t = p->cur_token; t.type != EOF_T; t = p->cur_token) {
        cur_stmt = parse_statement(p);
        // Just make the program contintue with the next statement 
        if (cur_stmt.type == NULL_STMT) {
            next_parser_token(p);
            continue;
        }
        append_stmt_list(&prog, cur_stmt);

        // Skip over semicolon
        next_parser_token(p);
    }

    return prog;
}


char* program_string(stmt_list* p) {
    char* stmt_str = malloc(2048);
    char* cur_str; 
    int str_pos = 0;

    // Just tack together all the statement strings
    for (int i = 0; i < p->count; i++) {
        cur_str = statement_string(&p->statements[i]);
        strcpy(stmt_str + str_pos, cur_str);
        str_pos += strlen(cur_str);
    }
    
    return stmt_str;
}


#endif
