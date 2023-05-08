#include <assert.h>
#include <stdbool.h>
#include "../lexer/lexer.c"
#include "../parser/parser.c"

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
    assert(pre.operator.type == MINUS);

    assert(pre.right->type == LITERAL_EXPR);
    assert(pre.right->data.lit.type == INT_LIT);
    literal lit = pre.right->data.lit;
    assert(lit.type == INT_LIT);
    assert(lit.data.i == 12);

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
    assert(pre.operator.type == MINUS);

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
    assert(ex->data.inf.operator.type == ASTERISK);

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

    assert(strcmp(prog_str, "((-12 * 5) + ((3 * -5) * 3))"));

    printf("test_infix_expr_2: %s\n", prog_str);
    free(prog_str);

    return true;
}

int main() {
    assert(test_program_string());
    test_identifier_expr();
    test_int_expr();
    test_prefix_expr();
    test_infix_expr();
    test_infix_expr_2();
    return 0;
}
