#include "../parser/parser.c"
#include <assert.h>
#include "eval.c"

object get_prog_output(char* input_str) {
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    env_map* env = new_env_map();
    object out = eval_program(&prog, env);
    free(env);

    return out;
}

bool assert_prog_output(char* input_str, literal exp_val) {
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    env_map* env = new_env_map();
    object out = eval_program(&prog, env);
    free(env);

    printf("%s: %s\n", input_str, literal_string(out.lit));

    assert(out.lit.type == exp_val.type);
    switch (out.lit.type) {
        case INT_LIT:
            assert(out.lit.data.i == exp_val.data.i);
            break;
        case BOOL_LIT:
            assert(out.lit.data.b == exp_val.data.b);
            break;
        case NULL_LIT:
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

    exp_val.type = INT_LIT;
    exp_val.data.i = 35;
    assert_prog_output("5 * 5 + 10;", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 75;
    assert_prog_output("5 * (5 + 10);", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 20;
    assert_prog_output("(100 / 2 + 10) / 3;", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = -5;
    assert_prog_output("-10 + 25 - 20;", exp_val);
}

bool test_eval_ifelse() {
    literal exp_val;

    exp_val.type = INT_LIT;
    exp_val.data.i = 10;
    assert_prog_output("if (5 == 5) { 10 } else { -10 };", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = -10;
    assert_prog_output("if (5 != 5) { 10 } else { -10 };", exp_val);

    exp_val.type = NULL_LIT;
    assert_prog_output("if (5 != 5) { 10 };", exp_val);
}

bool test_eval_return() {
    literal exp_val;

    exp_val.type = INT_LIT;
    exp_val.data.i = 10;
    char input_str[] = 
    "if (10 > 1) {"
    "    if (10 > 1) {"
    "        return 10;"
    "    }"
    "    return 1;"
    "}";
    assert_prog_output(input_str, exp_val);
}

bool test_eval_error() {
    char input_str_1[] = "5 == false;";
    object out = get_prog_output(input_str_1);
    assert(out.type == ERR_OBJ);
    assert(strcmp("type mismatch: Int != Bool", out.err) == 0);
    printf("%s: %s", input_str_1, out.err);

    char input_str_2[] = "true + false;";
    out = get_prog_output(input_str_2);
    assert(out.type == ERR_OBJ);
    assert(strcmp("operator not supported: +", out.err) == 0);
    printf("%s: %s", input_str_2, out.err);

    char input_str_3[] = "false / true";
    out = get_prog_output(input_str_3);
    assert(out.type == ERR_OBJ);
    assert(strcmp("operator not supported: /", out.err) == 0);
    printf("%s: %s", input_str_3, out.err);

    char input_str_4[] = "-false";
    out = get_prog_output(input_str_4);
    assert(out.type == ERR_OBJ);
    assert(strcmp("operator not supported: -", out.err) == 0);
    printf("%s: %s", input_str_4, out.err);
}

int main() {
    test_eval_int();
    test_eval_let();
    test_eval_prefix();
    test_eval_infix();
    test_eval_ifelse();
    test_eval_return();
    test_eval_error();
    return 0;
}