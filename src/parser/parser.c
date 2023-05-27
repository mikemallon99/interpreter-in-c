#include <stdio.h>

#include "parser.h"


void next_parser_token(parser* p) {
    p->cur_token = p->peek_token;
    p->peek_token = next_lexer_token(p->l);
}


void peek_error(parser* p, token_type cur_type) {
    sprintf(p->errors[p->num_errors], "Expected next token to be %s but got %s instead.", get_token_type_string(cur_type), get_token_type_string(p->peek_token.type));
    printf("ERROR: Expected next token to be %s but got %s instead.\n", get_token_type_string(cur_type), get_token_type_string(p->peek_token.type));
    p->num_errors++;
}


bool cur_token_is(parser* p, token_type cur_type) {
    return p->cur_token.type == cur_type;
}


bool peek_token_is(parser* p, token_type peek_type) {
    return p->peek_token.type == peek_type;
}


precedence peek_precedence(parser* p) {
    return get_precedence(p->peek_token);
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
        case TRUE:
            lit.type = BOOL_LIT;
            lit.data.b = true;
            break;
        case FALSE:
            lit.type = BOOL_LIT;
            lit.data.b = false;
            break;
        default:
            lit.type = NULL_LIT;
            break;
    }

    ex->data.lit = lit;
    return ex;
}


expr* parse_prefix(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = PREFIX_EXPR;
    struct prefix_expr pre;

    pre.op = p->cur_token;

    next_parser_token(p);

    pre.right = parse_expression(p, PREFIX_PR);

    ex->data.pre = pre;
    return ex;
}


expr* parse_infix(parser* p, expr* left) {
    expr* ex = malloc(sizeof(expr));
    ex->type = INFIX_EXPR;
    struct infix_expr inf;

    inf.op = p->cur_token;
    inf.left = left;

    next_parser_token(p);

    inf.right = parse_expression(p, get_precedence(inf.op));

    ex->data.inf = inf;
    return ex;
}


expr* parse_group(parser* p) {
    next_parser_token(p);

    expr* ex = parse_expression(p, LOWEST_PR);

    if (!expect_peek(p, RPAREN)) {
        return NULL;
    }

    return ex;
}


stmt_list parse_block_stmt(parser* p) {
    stmt_list block = new_stmt_list();
    stmt cur_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    for (token t = p->cur_token; t.type != RBRACE; t = p->cur_token) {
        cur_stmt = parse_statement(p);
        // Just make the program contintue with the next statement 
        if (cur_stmt.type == NULL_STMT) {
            next_parser_token(p);
            continue;
        }
        append_stmt_list(&block, cur_stmt);

        // Skip over semicolon
        next_parser_token(p);
    }

    return block;
}


expr* parse_if(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = IF_EXPR;
    struct if_expr if_data;

    if (!expect_peek(p, LPAREN)) {
        return NULL;
    }
    next_parser_token(p);

    if_data.condition = parse_expression(p, LOWEST_PR);

    if (!expect_peek(p, RPAREN)) {
        return NULL;
    }
    if (!expect_peek(p, LBRACE)) {
        return NULL;
    }
    next_parser_token(p);

    if_data.consequence = parse_block_stmt(p);

    // And then if theres an "else" statement
    if (peek_token_is(p, ELSE)) {
        next_parser_token(p);
        if (!expect_peek(p, LBRACE)) {
            return NULL;
        }
        next_parser_token(p);

        if_data.alternative = parse_block_stmt(p);
        if_data.has_alt = true;
    } else {
        if_data.has_alt = false;
    }

    ex->data.ifelse = if_data;
    return ex;
}


token_list parse_fn_params(parser* p) {
    token_list params = new_token_list();

    if (peek_token_is(p, RPAREN)) {
        next_parser_token(p);
        return params;
    }

    while (true) {
        if (!expect_peek(p, IDENT)) {
            return params;
        }
        append_token_list(&params, p->cur_token);

        if (peek_token_is(p, COMMA)) {
            next_parser_token(p);
        } else if (!expect_peek(p, RPAREN)) {
            return params;
        } else {
            return params;
        }
    }
}


expr_list parse_call_args(parser* p) {
    expr_list params = new_expr_list();

    if (peek_token_is(p, RPAREN)) {
        next_parser_token(p);
        return params;
    }

    while (true) {
        next_parser_token(p);
        append_expr_list(&params, parse_expression(p, LOWEST_PR));

        if (peek_token_is(p, COMMA)) {
            next_parser_token(p);
        } else if (!expect_peek(p, RPAREN)) {
            return params;
        } else {
            return params;
        }
    }
}


expr* parse_call(parser* p, expr* left) {
    expr* ex = malloc(sizeof(expr));
    ex->type = CALL_EXPR;
    struct call_expr call;

    call.func = left;

    call.args = parse_call_args(p);

    ex->data.call = call;
    return ex;
}


expr* parse_fn(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    literal lit;
    lit.type = FN_LIT;
    struct fn_lit fn_data;

    if (!expect_peek(p, LPAREN)) {
        return NULL;
    }

    fn_data.params = parse_fn_params(p);
    if (!expect_peek(p, LBRACE)) {
        return NULL;
    }
    next_parser_token(p);

    fn_data.body = parse_block_stmt(p);
    // Create an environment for functions to support closures
    // closure_fn = fn(x) { fn(y) { x + y }}
    fn_data.env = calloc(1, sizeof(environment));

    lit.data.fn = fn_data;
    ex->data.lit = lit;
    return ex;
}


expr* parse_expression(parser* p, precedence prec) {
    // Inital prefix function
    expr* left_expr;
    switch (p->cur_token.type) {
        // Literals
        case IDENT:
        case INT:
        case TRUE:
        case FALSE:
            left_expr = parse_literal(p);
            break;
        case MINUS:
        case BANG:
            left_expr = parse_prefix(p);
            break;
        case LPAREN:
            left_expr = parse_group(p);
            break;
        case IF:
            left_expr = parse_if(p);
            break;
        case FUNCTION:
            left_expr = parse_fn(p);
            break;
        default:
            left_expr = NULL;
    }

    while (!peek_token_is(p, SEMICOLON) && prec < peek_precedence(p)) {
        next_parser_token(p);
        switch (p->cur_token.type) {
            case LPAREN:
                left_expr = parse_call(p, left_expr);
                break;
            default:
                left_expr = parse_infix(p, left_expr);
                break;
        }
    }

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
