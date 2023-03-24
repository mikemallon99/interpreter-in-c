#include "../ast/ast.c"
#include "../lexer/lexer.c"

typedef struct {
    lexer* l;
    token cur_token;
    token peek_token;
} parser; 

void next_parser_token(parser* p) {
    p->cur_token = p->peek_token;
    p->peek_token = next_lexer_token(p->l);
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
        return false;
    }
}

parser new_parser(lexer* l) {
    parser p;
    p.l = l;

    // Call this twice to populate cur_token
    next_parser_token(&p);
    next_parser_token(&p);

    return p;
}

statement parse_let_statement(parser* p) {
    smt_let_data* data = calloc(1, sizeof(smt_let_data));
    
    // TODO: Verify next token is an identifier
}

statement parse_statement(parser* p) {
    switch (p->cur_token.tokenType) {
        case LET:
            
    }
}

program* parse_program(parser* p) {
    return NULL;
}
