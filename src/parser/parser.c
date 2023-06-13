#include <stdio.h>
#include <string.h>

#include "parser.h"

void _peek_error(parser* p, token_type cur_type);
void _error_string(parser* p, char* err_str);
bool _cur_token_is(parser* p, token_type cur_type);
bool _peek_token_is(parser* p, token_type peek_type);
precedence _peek_precedence(parser* p);
bool _expect_peek(parser* p, token_type peek_type);
expr* _parse_map(parser* p);
expr* _parse_literal(parser* p);
expr* _parse_prefix(parser* p);
expr* _parse_infix(parser* p, expr* left);
expr* _parse_group(parser* p);
stmt_list _parse_block_stmt(parser* p);
expr* _parse_if(parser* p);
token_list _parse_fn_params(parser* p);
expr_list _parse_call_args(parser* p);
expr* _parse_call(parser* p, expr* left);
expr* _parse_fn(parser* p);
expr* _parse_expression(parser* p, precedence prec);
stmt _parse_let_statement(parser* p);
stmt _parse_return_statement(parser* p);
stmt _parse_expr_stmt(parser* p);
stmt _parse_statement(parser* p);
void _next_parser_token(parser* p);

// PUBLIC FUNCTIONS

parser new_parser(lexer* l)
{
    parser p;
    p.l = l;
    p.num_errors = 0;

    // Call this twice to populate cur_token
    _next_parser_token(&p);
    _next_parser_token(&p);

    return p;
}

bool check_parser_errors(parser* p)
{
    if (p->num_errors == 0)
        return true;

    printf("Parser has %d errors:\n", p->num_errors);
    for (int i = 0; i < p->num_errors; i++)
    {
        printf("Error: %s\n", p->errors[i]);
    }
    return false;
}

stmt_list parse_program(parser* p)
{
    stmt_list prog = new_stmt_list();
    stmt cur_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    for (token t = p->cur_token; t.type != EOF_T; t = p->cur_token)
    {
        cur_stmt = _parse_statement(p);
        // Just make the program contintue with the next statement
        if (cur_stmt.type == NULL_STMT)
        {
            return prog;
        }
        append_stmt_list(&prog, cur_stmt);

        // Skip over semicolon
        _next_parser_token(p);
    }

    return prog;
}

// PRIVATE FUNCTIONS

void _peek_error(parser* p, token_type cur_type)
{
    char* exp_token_str = get_token_type_string(cur_type);
    char* actual_token_str = get_token_type_string(p->peek_token.type);

    sprintf(p->errors[p->num_errors], "Expected next token to be %s but got %s instead.", exp_token_str, actual_token_str);
    // printf("ERROR: Expected next token to be %s but got %s instead.\n", exp_token_str, actual_token_str);

    free(exp_token_str);
    free(actual_token_str);
    p->num_errors++;
}

void _error_string(parser* p, char* err_str)
{
    strcpy(p->errors[p->num_errors], err_str);
    p->num_errors++;
}

bool _cur_token_is(parser* p, token_type cur_type)
{
    return p->cur_token.type == cur_type;
}

bool _peek_token_is(parser* p, token_type peek_type)
{
    return p->peek_token.type == peek_type;
}

precedence _peek_precedence(parser* p)
{
    return get_precedence(p->peek_token);
}

bool _expect_peek(parser* p, token_type peek_type)
{
    if (_peek_token_is(p, peek_type))
    {
        _next_parser_token(p);
        return true;
    }
    else
    {
        _peek_error(p, peek_type);
        return false;
    }
}

expr* _parse_literal(parser* p)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    literal lit;

    switch (p->cur_token.type)
    {
    case IDENT:
        lit.type = IDENT_LIT;
        lit.data.t = p->cur_token;
        break;
    case INT:
        lit.type = INT_LIT;
        lit.data.i = atoi(p->cur_token.value);
        free(p->cur_token.value);
        break;
    case TRUE:
        lit.type = BOOL_LIT;
        lit.data.b = true;
        free(p->cur_token.value);
        break;
    case FALSE:
        lit.type = BOOL_LIT;
        lit.data.b = false;
        free(p->cur_token.value);
        break;
    case STRING:
        lit.type = STRING_LIT;
        lit.data.s = p->cur_token.value;
        break;
    default:
        lit.type = NULL_LIT;
        free(p->cur_token.value);
        break;
    }

    ex->data.lit = lit;
    return ex;
}

expr* _parse_prefix(parser* p)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = PREFIX_EXPR;
    struct prefix_expr pre;

    pre.op = p->cur_token;

    _next_parser_token(p);

    pre.right = _parse_expression(p, PREFIX_PR);
    if (pre.right == NULL) {
        _error_string(p, "Prefix has invalid right expression.");
        return NULL;
    }

    ex->data.pre = pre;
    return ex;
}


expr* _parse_infix(parser* p, expr* left)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = INFIX_EXPR;
    struct infix_expr inf;

    inf.op = p->cur_token;
    inf.left = left;

    _next_parser_token(p);

    if (inf.op.type == LBRACKET) {
        // Need to be able to parse expressions inside of brackets
        inf.right = _parse_expression(p, LOWEST_PR);
        if (inf.right == NULL) {
            _error_string(p, "Infix has invalid right expression.");
            return NULL;
        }
        // Array index infix will have a ] at the end
        if (!_expect_peek(p, RBRACKET))
        {
            return NULL;
        }
    }
    else {
        inf.right = _parse_expression(p, get_precedence(inf.op));
        if (inf.right == NULL) {
            _error_string(p, "Infix has invalid right expression.");
            return NULL;
        }
    }

    ex->data.inf = inf;
    return ex;
}

expr* _parse_group(parser* p)
{
    _next_parser_token(p);

    expr* ex = _parse_expression(p, LOWEST_PR);
    if (ex == NULL) {
        _error_string(p, "Group contains invalid expression.");
        return NULL;
    }

    if (!_expect_peek(p, RPAREN))
    {
        return NULL;
    }

    return ex;
}

stmt_list _parse_block_stmt(parser* p)
{
    stmt_list block = new_stmt_list();
    stmt cur_stmt;

    // Need to do first iteration so we have our "prev_stmt" initialized
    while (p->cur_token.type != RBRACE)
    {
        cur_stmt = _parse_statement(p);
        // Just make the program contintue with the next statement
        if (cur_stmt.type == NULL_STMT)
        {
            _expect_peek(p, RBRACE);
            return block;
        }
        append_stmt_list(&block, cur_stmt);

        // Skip over semicolon
        _next_parser_token(p);
    }

    return block;
}

expr* _parse_if(parser* p)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = IF_EXPR;
    struct if_expr if_data;

    if (!_expect_peek(p, LPAREN))
    {
        return NULL;
    }

    _next_parser_token(p);

    if_data.condition = _parse_expression(p, LOWEST_PR);
    if (if_data.condition == NULL) {
        _error_string(p, "If condition contains invalid expression.");
        return NULL;
    }

    if (!_expect_peek(p, RPAREN))
    {
        return NULL;
    }

    if (!_expect_peek(p, LBRACE))
    {
        return NULL;
    }

    _next_parser_token(p);

    if_data.consequence = _parse_block_stmt(p);

    // And then if theres an "else" statement
    if (_peek_token_is(p, ELSE))
    {
        _next_parser_token(p);

        if (!_expect_peek(p, LBRACE))
        {
            return NULL;
        }

        _next_parser_token(p);

        if_data.alternative = _parse_block_stmt(p);
        if_data.has_alt = true;
    }
    else
    {
        if_data.has_alt = false;
    }

    ex->data.ifelse = if_data;
    return ex;
}

token_list _parse_fn_params(parser* p)
{
    token_list params = new_token_list();

    if (_peek_token_is(p, RPAREN))
    {
        _next_parser_token(p);
        return params;
    }

    while (true)
    {
        if (!_expect_peek(p, IDENT))
        {
            return params;
        }
        append_token_list(&params, p->cur_token);

        if (_peek_token_is(p, COMMA))
        {
            _next_parser_token(p);
        }
        else if (!_expect_peek(p, RPAREN))
        {
            return params;
        }
        else 
        {
            return params;
        }
    }
}

expr_list _parse_call_args(parser* p)
{
    expr_list params = new_expr_list();

    if (_peek_token_is(p, RPAREN))
    {
        _next_parser_token(p);
        return params;
    }

    while (true)
    {
        _next_parser_token(p);
        expr* new_expr = _parse_expression(p, LOWEST_PR);
        if (new_expr == NULL) {
            _error_string(p, "Call arguments contains invalid expression.");
            return params;
        }
        append_expr_list(&params, new_expr);

        if (_peek_token_is(p, COMMA))
        {
            _next_parser_token(p);
        }
        else if (!_expect_peek(p, RPAREN))
        {
            return params;
        }
        else {
            return params;
        }
    }
}

expr* _parse_call(parser* p, expr* left)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = CALL_EXPR;
    struct call_expr call;

    call.func = left;

    call.args = _parse_call_args(p);

    ex->data.call = call;
    return ex;
}

expr* _parse_fn(parser* p)
{
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    literal lit;
    lit.type = FN_LIT;
    struct fn_lit fn_data;

    if (!_expect_peek(p, LPAREN))
    {
        return NULL;
    }

    fn_data.params = _parse_fn_params(p);
    if (!_expect_peek(p, LBRACE))
    {
        return NULL;
    }

    _next_parser_token(p);

    fn_data.body = _parse_block_stmt(p);
    fn_data.env = NULL;

    lit.data.fn = fn_data;
    ex->data.lit = lit;
    return ex;
}

expr* _parse_array(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    ex->data.lit.type = ARRAY_LIT;
    ex->data.lit.data.arr = new_expr_list();

    if (_peek_token_is(p, RBRACKET))
    {
        _next_parser_token(p);
        return ex;
    }

    while (true)
    {
        _next_parser_token(p);
        expr* new_expr = _parse_expression(p, LOWEST_PR);
        if (new_expr == NULL) {
            _error_string(p, "Array member is an invalid expression.");
            return NULL;
        }
        append_expr_list(&ex->data.lit.data.arr, new_expr);

        if (_peek_token_is(p, COMMA))
        {
            _next_parser_token(p);
        }
        else if (!_expect_peek(p, RBRACKET))
        {
            return NULL;
        }
        else {
            return ex;
        }
    }
}

expr* _parse_map(parser* p) {
    expr* ex = malloc(sizeof(expr));
    ex->type = LITERAL_EXPR;
    ex->data.lit.type = MAP_LIT;
    ex->data.lit.data.map = new_expr_pair_list();

    if (_peek_token_is(p, RBRACE))
    {
        _next_parser_token(p);
        return ex;
    }

    while (true)
    {
        _next_parser_token(p);

        expr_pair new_pair;
        new_pair.first = _parse_expression(p, LOWEST_PR);
        if (new_pair.first == NULL) {
            _error_string(p, "Map key is an invalid expression.");
            return NULL;
        }

        if (!_expect_peek(p, COLON))
        {
            return NULL;
        }

        _next_parser_token(p);

        new_pair.second = _parse_expression(p, LOWEST_PR);
        if (new_pair.second == NULL) {
            _error_string(p, "Map value is an invalid expression.");
            return NULL;
        }

        append_expr_pair_list(&ex->data.lit.data.map, new_pair);

        if (_peek_token_is(p, COMMA))
        {
            _next_parser_token(p);
        }
        else if (!_expect_peek(p, RBRACE))
        {
            return NULL;
        }
        else {
            return ex;
        }
    }

    return ex;
}

expr* _parse_expression(parser* p, precedence prec)
{
    // Inital prefix function
    expr* left_expr;
    switch (p->cur_token.type)
    {
        // Literals
        case IDENT:
        case INT:
        case TRUE:
        case FALSE:
        case STRING:
            left_expr = _parse_literal(p);
            break;
        case MINUS:
        case BANG:
            left_expr = _parse_prefix(p);
            break;
        case LPAREN:
            left_expr = _parse_group(p);
            break;
        case IF:
            left_expr = _parse_if(p);
            break;
        case FUNCTION:
            left_expr = _parse_fn(p);
            break;
        case LBRACKET:
            left_expr = _parse_array(p);
            break;
        case LBRACE:
            left_expr = _parse_map(p);
            break;
        default:
            left_expr = NULL;
            _error_string(p, "Did not find expression function for token.");
    }

    if (left_expr == NULL) {
        return left_expr;
    }

    while (!_peek_token_is(p, SEMICOLON) && prec < _peek_precedence(p))
    {
        _next_parser_token(p);
        switch (p->cur_token.type)
        {
            case LPAREN:
                left_expr = _parse_call(p, left_expr);
                break;
            default:
                left_expr = _parse_infix(p, left_expr);
                break;
        }
    }

    return left_expr;
}

stmt _parse_let_statement(parser* p)
{
    let_stmt let;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;

    // Verify next token is an identifier
    if (!_expect_peek(p, IDENT))
    {
        return new_stmt;
    }
    let.identifier = p->cur_token;

    // Verify next token is an equals sign
    if (!_expect_peek(p, ASSIGN))
    {
        return new_stmt;
    }

    _next_parser_token(p);

    let.value = _parse_expression(p, LOWEST_PR);
    if (let.value == NULL) {
        _error_string(p, "Let statement assigns to invalid expression.");
        return new_stmt;
    }

    if (_peek_token_is(p, SEMICOLON))
    {
        _next_parser_token(p);
    }

    new_stmt.type = LET_STMT;
    new_stmt.data.let = let;
    return new_stmt;
}

stmt _parse_return_statement(parser* p)
{
    return_stmt ret;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;

    _next_parser_token(p);

    ret.value = _parse_expression(p, LOWEST_PR);
    if (ret.value == NULL) {
        _error_string(p, "Return statement returns invalid expression.");
        return new_stmt;
    }

    if (_peek_token_is(p, SEMICOLON))
    {
        _next_parser_token(p);
    }

    new_stmt.type = RETURN_STMT;
    new_stmt.data.ret = ret;
    return new_stmt;
}

stmt _parse_expr_stmt(parser* p)
{
    expr_stmt ex;
    stmt new_stmt;
    new_stmt.type = NULL_STMT;

    ex.value = _parse_expression(p, LOWEST_PR);
    if (ex.value == NULL) {
        _error_string(p, "Expression statement has invalid expression.");
        return new_stmt;
    }

    if (_peek_token_is(p, SEMICOLON))
    {
        _next_parser_token(p);
    }

    new_stmt.type = EXPR_STMT;
    new_stmt.data.expr = ex;
    return new_stmt;
}

stmt _parse_statement(parser* p)
{
    switch (p->cur_token.type)
    {
        case LET:
            return _parse_let_statement(p);
        case RETURN:
            return _parse_return_statement(p);
        default:
            return _parse_expr_stmt(p);
    }
}

void _next_parser_token(parser *p)
{
    p->cur_token = p->peek_token;
    p->peek_token = next_lexer_token(p->l);
}
