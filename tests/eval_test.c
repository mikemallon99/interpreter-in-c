#ifndef _EVALTESTC_
#define _EVALTESTC_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/parser/parser.h"
#include "../src/eval/eval.h"

object get_prog_output(char* input_str)
{
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    environment env;
    env_map inner = new_env_map();
    env.inner = inner;
    env.outer = NULL;
    object out = eval_program(&prog, &env);
    cleanup_environment(&env);

    return out;
}

bool assert_prog_output(char* input_str, literal exp_val)
{
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    environment env;
    env_map inner = new_env_map();
    env.inner = inner;
    env.outer = NULL;
    object out = eval_program(&prog, &env);

    char* out_str = literal_string(out.lit);
    printf("%s: %s\n", input_str, out_str);
    free(out_str);

    assert(out.lit.type == exp_val.type);
    switch (out.lit.type)
    {
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

    cleanup_stmt_list(prog);
    force_cleanup_environment(&env);
    cleanup_object(out);
}

bool test_eval_int()
{
    literal exp_val;
    exp_val.type = INT_LIT;
    exp_val.data.i = 5;
    assert_prog_output("5;", exp_val);
}

bool test_eval_let()
{
    literal exp_val;
    exp_val.type = INT_LIT;
    exp_val.data.i = 5;
    assert_prog_output("let five = 5; five;", exp_val);
}

bool test_eval_prefix()
{
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

bool test_eval_infix()
{
    literal exp_val;

    exp_val.type = INT_LIT;
    exp_val.data.i = 35;
    assert_prog_output("5*  5 + 10;", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 75;
    assert_prog_output("5*  (5 + 10);", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = 20;
    assert_prog_output("(100 / 2 + 10) / 3;", exp_val);

    exp_val.type = INT_LIT;
    exp_val.data.i = -5;
    assert_prog_output("-10 + 25 - 20;", exp_val);
}

bool test_eval_ifelse()
{
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

bool test_eval_return()
{
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

bool test_eval_error()
{
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

    char input_str_5[] = "if (false / true) { 10 }";
    out = get_prog_output(input_str_5);
    assert(out.type == ERR_OBJ);
    assert(strcmp("operator not supported: /", out.err) == 0);
    printf("%s: %s", input_str_5, out.err);
}

bool test_eval_fn()
{
    char input_str_1[] =
        "let basic_fn = fn(x){ x + 2 };"
        "basic_fn(2);";

    literal exp_val_1;
    exp_val_1.type = INT_LIT;
    exp_val_1.data.i = 4;
    assert_prog_output(input_str_1, exp_val_1);

    char input_str_2[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "add_2(4);";

    literal exp_val_2;
    exp_val_2.type = INT_LIT;
    exp_val_2.data.i = 6;
    assert_prog_output(input_str_2, exp_val_2);
}

bool test_eval_fn_2()
{
    char input_str[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "let add_3 = closure_fn(3);"
        "add_2(4) + add_3(3);";
    literal exp_val;

    exp_val.type = INT_LIT;
    exp_val.data.i = 6 + 6;
    assert_prog_output(input_str, exp_val);

    char input_str_2[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "let add_3 = closure_fn(3);"
        "add_2(4) + add_3(3);";
    literal exp_val_2;

    exp_val_2.type = INT_LIT;
    exp_val_2.data.i = 6 + 6;
    assert_prog_output(input_str_2, exp_val_2);
}

bool test_eval_fn_rec()
{
    char input_str[] =
        "let factorial = fn(x){ if (x == 0) {return 1;} else {return x * factorial(x - 1);}; };"
        "factorial(3);";
    literal exp_val;

    exp_val.type = INT_LIT;
    exp_val.data.i = 6;
    assert_prog_output(input_str, exp_val);
}

void run_all_eval_tests()
{
    test_eval_int();
    test_eval_let();
    test_eval_prefix();
    test_eval_infix();
    test_eval_ifelse();
    test_eval_return();
    test_eval_error();
    test_eval_fn();
    test_eval_fn_2();
    test_eval_fn_rec();
}

#endif
