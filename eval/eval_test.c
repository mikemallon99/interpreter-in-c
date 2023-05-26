#include "../parser/parser.c"
#include <assert.h>
#include "eval.c"

bool assert_prog_output(char* input_str, literal exp_val) {
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    literal out = eval_program(&prog);

    printf("%s: %s\n", input_str, literal_string(out));

    assert(out.type == exp_val.type);
    switch (out.type) {
        case INT_LIT:
            assert(out.data.i == exp_val.data.i);
            break;
        case BOOL_LIT:
            assert(out.data.b == exp_val.data.b);
            break;
        default:
            assert(false);
    }
}

bool test_eval_int() {
    literal exp_val;
    exp_val.type = INT_LIT;
    exp_val.data.i = 5;
    assert_prog_output("5;", exp_val);
}

bool test_eval_let() {
    literal exp_val;
    exp_val.type = INT_LIT;
    exp_val.data.i = 5;
    assert_prog_output("let five = 5; five;", exp_val);
}

bool test_eval_prefix() {
    literal exp_val;

    // EVAL BANGS
    exp_val.type = BOOL_LIT;
    exp_val.data.b = false;
    assert_prog_output("!5;", exp_val);

    exp_val.type = BOOL_LIT;
    exp_val.data.b = true;
    assert_prog_output("!!5;", exp_val);

    exp_val.type = BOOL_LIT;
    exp_val.data.b = true;
    assert_prog_output("!false;", exp_val);

    // EVAL MINUS
    exp_val.type = INT_LIT;
    exp_val.data.i = -5;
    assert_prog_output("-5;", exp_val);
}

bool test_eval_infix() {
    literal exp_val;

    // EVAL BANGS
    exp_val.type = INT_LIT;
    exp_val.data.i = 35;
    assert_prog_output("5 * 5 + 10;", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 75;
    assert_prog_output("5 * (5 + 10);", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 20;
    assert_prog_output("(100 / 2 + 10) / 3;", exp_val);

    // EVAL MINUS
    exp_val.type = INT_LIT;
    exp_val.data.i = -5;
    assert_prog_output("-10 + 25 - 20;", exp_val);
}

int main() {
    test_eval_int();
    test_eval_let();
    test_eval_prefix();
    test_eval_infix();
    return 0;
}