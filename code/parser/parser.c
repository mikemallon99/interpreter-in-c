#include "../ast/ast.c"
#include "../lexer/lexer.c"
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

statement* parse_let_statement(parser* p) {
    statement* stmt = calloc(1, sizeof(statement));
    stmt->type = SMT_LET;
    smt_let_data* data = calloc(1, sizeof(smt_let_data));
    stmt->data = data;
    stmt->next = NULL;

    // Let token
    data->t = p->cur_token;
    
    // Verify next token is an identifier
    if (!expect_peek(p, IDENT)) {
        stmt->data = NULL;
        return stmt;
    }
    data->ident = p->cur_token;

    // Verify next token is an equals sign
    if (!expect_peek(p, ASSIGN)) {
        stmt->data = NULL;
        return stmt;
    }

    // Skip over the expressions for now
    while (p->cur_token.tokenType != SEMICOLON) {
        next_parser_token(p);
    }

    return stmt;
}

statement* parse_return_statement(parser* p) {
    statement* stmt = calloc(1, sizeof(statement));
    stmt->type = SMT_RETURN;
    smt_return_data* data = calloc(1, sizeof(smt_return_data));
    stmt->data = data;
    stmt->next = NULL;

    // Return token
    data->t = p->cur_token;
    
    // Skip over the expressions for now
    while (p->cur_token.tokenType != SEMICOLON) {
        next_parser_token(p);
    }

    return stmt;
}

statement* parse_statement(parser* p) {
    switch (p->cur_token.tokenType) {
        case LET:
            return parse_let_statement(p);
        case RETURN:
            return parse_return_statement(p);
        default:
            return NULL;
    }
}

program* parse_program(parser* p) {
    program* prog = calloc(1, sizeof(program));
    statement* cur_stmt;
    statement* prev_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    cur_stmt = parse_statement(p);
    if (cur_stmt == NULL) {
        return NULL;
    }
    prog->statements = cur_stmt;
    prog->num_statements = 1;
    prev_stmt = cur_stmt;
    next_parser_token(p);

    for (token t = p->cur_token; t.tokenType != EOF_T; t = p->cur_token) {
        cur_stmt = parse_statement(p);
        // Just make the program contintue with the next statement 
        if (cur_stmt == NULL) {
            next_parser_token(p);
            continue;
        }
        prev_stmt->next = cur_stmt;
        prog->num_statements++;
        prev_stmt = cur_stmt;
        // Skip over semicolon
        next_parser_token(p);
    }

    return prog;
}
