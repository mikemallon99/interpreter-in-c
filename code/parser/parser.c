#ifndef _PARSERC_
#define _PARSERC_

#include "../lexer/lexer.c"
#include "statements.c"
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
    sprintf(p->errors[p->num_errors], "Expected next token to be %s but got %s instead.", get_token_type_string(cur_type), get_token_type_string(p->peek_token.tokenType));
    p->num_errors++;
}

bool cur_token_is(parser* p, token_type cur_type) {
    return p->cur_token.tokenType == cur_type;
}

bool peek_token_is(parser* p, token_type peek_type) {
    return p->peek_token.tokenType == peek_type;
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

    // Skip over the expressions for now
    while (p->cur_token.tokenType != SEMICOLON) {
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
    
    // Skip over the expressions for now
    while (p->cur_token.tokenType != SEMICOLON) {
        next_parser_token(p);
    }

    new_stmt.type = RETURN_STMT;
    new_stmt.data.ret = ret;
    return new_stmt;
}

stmt parse_statement(parser* p) {
    stmt null_stmt;
    null_stmt.type = NULL_STMT;
    switch (p->cur_token.tokenType) {
        case LET:
            return parse_let_statement(p);
        case RETURN:
            return parse_return_statement(p);
        default:
            return null_stmt;
    }
}

stmt_list parse_program(parser* p) {
    stmt_list prog;
    stmt cur_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    for (token t = p->cur_token; t.tokenType != EOF_T; t = p->cur_token) {
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

#endif
