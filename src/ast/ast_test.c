#include <assert.h>
#include <stdbool.h>
#include "../lexer/lexer.h"
#include "../parser/parser.h"

bool test_program_string() {
    // Input string into program and assert that the same string comes out
    char input_str[] = "let my_var = another_var;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    char* output_str = program_string(&prog);
    if (strcmp(input_str, output_str) != 0) {
        printf("test_program_string: Input and output strings different:\n");
        printf("    input_str: %s\n", input_str);
        printf("    output_str: %s\n", output_str);
        return false;
    }
    printf("program look good:\n \t%s\n", output_str);

    return true;
}

bool test_identifier_expr() {
    char input_str[] = "foobar;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    expr* ex = ident_stmt.data.expr.value;
    assert(ex->type == LITERAL_EXPR);

    literal lit = ex->data.lit;
    assert(lit.type == IDENT_LIT);
    assert(strcmp(lit.data.t.value, "foobar") == 0);

    char* prog_str = program_string(&prog);
    printf("test_identifier_expr: %s\n", prog_str);
    free(prog_str);

    return true;
}

bool test_int_expr() {
    char input_str[] = "12;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    expr* ex = ident_stmt.data.expr.value;
    assert(ex->type == LITERAL_EXPR);

    literal lit = ex->data.lit;
    assert(lit.type == INT_LIT);
    assert(lit.data.i == 12);

    char* prog_str = program_string(&prog);
    printf("test_int_expr: %s\n", prog_str);
    free(prog_str);

    return true;
}

void test_int_literal(expr* ex, int expected) {
    assert(ex->type == LITERAL_EXPR);
    literal lit = ex->data.lit;
    assert(lit.type == INT_LIT);
    assert(lit.data.i == expected);
}

void test_ident_literal(expr* ex, char* expected) {
    assert(ex->type == LITERAL_EXPR);
    literal lit = ex->data.lit;
    assert(lit.type == IDENT_LIT);
    assert(strcmp(lit.data.t.value, expected) == 0);
}

void test_bool_literal(expr* ex, bool expected) {
    assert(ex->type == LITERAL_EXPR);
    literal lit = ex->data.lit;
    assert(lit.type == BOOL_LIT);
    assert(lit.data.b == expected);
}

bool test_bool_expr() {
    char input_str[] = "false;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    expr* ex = ident_stmt.data.expr.value;
    test_bool_literal(ex, false);

    char* prog_str = program_string(&prog);
    printf("test_bool_expr: %s\n", prog_str);
    free(prog_str);

    return true;
}

bool test_if_expr() {
    // IF ELSE
    char input_str[] = "if (1 > 2) { let i = 4; } else { let i = 0; }";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    char* prog_str = program_string(&prog);
    printf("test_if_expr IF ELSE: %s\n", prog_str);
    assert(strcmp(prog_str, "if ((1 > 2)) { let i = 4; } else { let i = 0; }") == 0);
    free(prog_str);

    // JUST IF
    char input_str_2[] = "if (1 > 2) { let i = 4; }";
    l = get_lexer(input_str_2);
    p = new_parser(&l);
    prog = parse_program(&p);
    assert(prog.count == 1);

    ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    prog_str = program_string(&prog);
    printf("test_if_expr JUST IF: %s\n", prog_str);
    assert(strcmp(prog_str, "if ((1 > 2)) { let i = 4; }") == 0);
    free(prog_str);

    return true;
}


bool test_fn_expr() {
    // IF ELSE
    char input_str[] = "fn (x, y) { return x * y; }";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    char* prog_str = program_string(&prog);
    printf("test_fn_expr: %s\n", prog_str);
    assert(strcmp(prog_str, "fn(x, y) { return (x * y); }") == 0);
    free(prog_str);

    return true;
}


bool test_call_expr_1() {
    // IF ELSE
    char input_str[] = "fn (x, y) { return x * y; } (1, 2)";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    char* prog_str = program_string(&prog);
    printf("test_fn_expr: %s\n", prog_str);
    assert(strcmp(prog_str, "fn(x, y) { return (x * y); }(1, 2)") == 0);
    free(prog_str);

    return true;
}


bool test_call_expr_2() {
    // IF ELSE
    char input_str[] = "2 * -test_fn()";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    char* prog_str = program_string(&prog);
    printf("test_fn_expr: %s\n", prog_str);
    assert(strcmp(prog_str, "(2 * -test_fn())") == 0);
    free(prog_str);

    return true;
}


bool test_prefix_expr() {
    char input_str[] = "-12;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    expr* ex = ident_stmt.data.expr.value;
    assert(ex->type == PREFIX_EXPR);

    struct prefix_expr pre = ex->data.pre;
    assert(pre.op.type == MINUS);

    test_int_literal(pre.right, 12);

    char* prog_str = program_string(&prog);
    printf("test_prefix_expr: %s\n", prog_str);
    free(prog_str);

    return true;
}

bool test_infix_expr() {
    char input_str[] = "-12 * 5;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    stmt ident_stmt = prog.statements[0];
    assert(ident_stmt.type == EXPR_STMT);

    expr* ex = ident_stmt.data.expr.value;
    assert(ex->type == INFIX_EXPR);

    // Left Side
    expr* left = ex->data.inf.left;
    assert(left->type == PREFIX_EXPR);

    struct prefix_expr pre = left->data.pre;
    assert(pre.op.type == MINUS);

    assert(pre.right->type == LITERAL_EXPR);
    assert(pre.right->data.lit.type == INT_LIT);
    literal lit_left = pre.right->data.lit;
    assert(lit_left.type == INT_LIT);
    assert(lit_left.data.i == 12);

    // Right Side
    expr* right = ex->data.inf.right;
    assert(right->type == LITERAL_EXPR);
    assert(right->data.lit.type == INT_LIT);
    literal lit_right = right->data.lit;
    assert(lit_right.type == INT_LIT);
    assert(lit_right.data.i == 5);

    // Operator
    assert(ex->data.inf.op.type == ASTERISK);

    char* prog_str = program_string(&prog);
    printf("test_infix_expr: %s\n", prog_str);
    free(prog_str);

    return true;
}

bool test_infix_expr_2() {
    char input_str[] = "-12 * 5 + 3 * -5 * 3;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    char* prog_str = program_string(&prog);

    assert(strcmp(prog_str, "((-12 * 5) + ((3 * -5) * 3))") == 0);

    printf("test_infix_expr_2: %s\n", prog_str);
    free(prog_str);

    return true;
}

int main() {
    assert(test_program_string());
    test_identifier_expr();
    test_int_expr();
    test_bool_expr();
    test_prefix_expr();
    test_infix_expr();
    test_infix_expr_2();
    test_if_expr();
    test_fn_expr();
    test_call_expr_1();
    test_call_expr_2();
    return 0;
}
