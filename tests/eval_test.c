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

object create_int_obj(int i)
{
    literal lit;
    lit.type = INT_LIT;
    lit.data.i = i;

    object new_obj;
    new_obj.type = LIT_OBJ;
    new_obj.lit = lit;

    return new_obj;
}

object create_array_obj(int arr[], int size)
{
    object new_obj;
    new_obj.type = ARRAY_OBJ;
    new_obj.arr = new_object_list();
    for (int i = 0; i < size; i++) {
        append_object_list(&new_obj.arr, create_int_obj(arr[i]));
    }

    return new_obj;
}

object create_bool_obj(bool b)
{
    literal lit;
    lit.type = BOOL_LIT;
    lit.data.b = b;

    object new_obj;
    new_obj.type = LIT_OBJ;
    new_obj.lit = lit;

    return new_obj;
}

object create_null_obj()
{
    literal lit;
    lit.type = NULL_LIT;

    object new_obj;
    new_obj.type = LIT_OBJ;
    new_obj.lit = lit;

    return new_obj;
}

object create_str_obj(char* str)
{
    literal lit;
    lit.type = STRING_LIT;
    lit.data.s = str;

    object new_obj;
    new_obj.type = LIT_OBJ;
    new_obj.lit = lit;

    return new_obj;
}

bool assert_prog_output(char* input_str, object exp_val)
{
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    environment env;
    env_map inner = new_env_map();
    env.inner = inner;
    env.outer = NULL;
    object out = eval_program(&prog, &env);

    char* out_str = object_string(out);
    printf("%s: %s\n", input_str, out_str);
    free(out_str);

    assert(is_object_equal(out, exp_val));

    cleanup_stmt_list(prog);
    force_cleanup_environment(&env);
    cleanup_object(out);
}

bool test_eval_int()
{
    assert_prog_output("5;", create_int_obj(5));
}

bool test_eval_let()
{
    assert_prog_output("let five = 5; five;", create_int_obj(5));
}

bool test_eval_prefix()
{
    assert_prog_output("!5;", create_bool_obj(false));
    assert_prog_output("!!5;", create_bool_obj(true));
    assert_prog_output("!false;", create_bool_obj(true));
    assert_prog_output("-5;", create_int_obj(-5));
}

bool test_eval_infix()
{
    assert_prog_output("5*  5 + 10;", create_int_obj(35));
    assert_prog_output("5*  (5 + 10);", create_int_obj(75));
    assert_prog_output("(100 / 2 + 10) / 3;", create_int_obj(20));
    assert_prog_output("-10 + 25 - 20;", create_int_obj(-5));
}

bool test_eval_ifelse()
{
    assert_prog_output("if (5 == 5) { 10 } else { -10 };", create_int_obj(10));
    assert_prog_output("if (5 != 5) { 10 } else { -10 };", create_int_obj(-10));
    assert_prog_output("if (5 != 5) { 10 };", create_null_obj());
}

bool test_eval_return()
{
    char input_str[] =
        "if (10 > 1) {"
        "    if (10 > 1) {"
        "        return 10;"
        "    }"
        "    return 1;"
        "}";
    object obj = create_int_obj(10);
    obj.type = RET_OBJ;
    assert_prog_output(input_str, obj);
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
    assert_prog_output(input_str_1, create_int_obj(4));

    char input_str_2[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "add_2(4);";
    assert_prog_output(input_str_2, create_int_obj(6));
}

bool test_eval_fn_2()
{
    char input_str[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "let add_3 = closure_fn(3);"
        "add_2(4) + add_3(3);";
    assert_prog_output(input_str, create_int_obj(12));

    char input_str_2[] =
        "let closure_fn = fn(x){ fn(y) {x + y}};"
        "let add_2 = closure_fn(2);"
        "let add_3 = closure_fn(3);"
        "add_2(4) + add_3(3);";
    assert_prog_output(input_str_2, create_int_obj(12));
}

bool test_eval_fn_rec()
{
    char input_str[] =
        "let factorial = fn(x){ if (x == 0) {return 1;} else {return x * factorial(x - 1);}; };"
        "factorial(3);";
    object obj = create_int_obj(6);
    obj.type = RET_OBJ;
    assert_prog_output(input_str, obj);
}

bool test_eval_string()
{
    char input_str[] =
        "let s = \"test string!!\";"
        "s;";
    assert_prog_output(input_str, create_str_obj("test string!!"));

    char input_str_2[] =
        "let one = \"one\";"
        "let two = \"two\";"
        "let three = \"three\";"
        "one + \" \" + two + \" \" + three;";
    assert_prog_output(input_str_2, create_str_obj("one two three"));
}

bool test_eval_builtin()
{
    char input_str[] =
        "let s = \"test string!!\";"
        "let s_length = len(s);"
        "s_length";
    assert_prog_output(input_str, create_int_obj(13));
}

bool test_eval_array()
{
    char input_str_1[] =
        "let arr = [1, 2, 3];"
        "arr[1]";
    assert_prog_output(input_str_1, create_int_obj(2));

    char input_str_2[] =
        "let arr = [1, 1 + 1, \"333\", fn() {4}];"
        "let out = arr[0] + arr[1] + len(arr[2]) + arr[3]();"
        "out";
    assert_prog_output(input_str_2, create_int_obj(10));

    char input_str_3[] =
        "let arr = [1, 2, 3];"
        "arr[2-1]";
    assert_prog_output(input_str_3, create_int_obj(2));

    // TEST BUILTINS
    char input_str_4[] =
        "let arr = [1, 2, 3];"
        "len(arr)";
    assert_prog_output(input_str_4, create_int_obj(3));

    char input_str_5[] =
        "let arr = [1, 2, 3];"
        "first(arr)";
    assert_prog_output(input_str_5, create_int_obj(1));

    char input_str_6[] =
        "let arr = [1, 2, 3];"
        "last(arr)";
    assert_prog_output(input_str_6, create_int_obj(3));

    char input_str_7[] =
        "let arr = [1, 2, 3, 4];"
        "rest(arr)";
    int arr_1[] = {2,3,4};
    assert_prog_output(input_str_7, create_array_obj(arr_1, 3));

    char input_str_8[] =
        "let arr = [1, 2, 3, 4];"
        "rest(rest(rest(arr)))";
    int arr_2[] = {4};
    assert_prog_output(input_str_8, create_array_obj(arr_2, 1));

    char input_str_9[] =
        "let a = [1, 2, 3, 4];"
        "let b = push(a, 5);"
        "b";
    int arr_3[] = {1, 2, 3, 4, 5};
    assert_prog_output(input_str_9, create_array_obj(arr_3, 5));

    char input_str_10[] =
        "let a = [1, 2, 3, 4];"
        "let b = push(a, 5);"
        "a";
    int arr_4[] = {1, 2, 3, 4};
    assert_prog_output(input_str_10, create_array_obj(arr_4, 4));

    char input_str_11[] = 
        "let map = fn(arr, f) { let iter = fn(arr, accumulated) { if (len(arr) == 0) { accumulated } else { iter(rest(arr), push(accumulated, f(first(arr)))); } } iter(arr, []); };"
        "let a = [1, 2, 3, 4];"
        "let double = fn(x) { x * 2 };"
        "map(a, double);";
    int arr_5[] = {2, 4, 6, 8};
    assert_prog_output(input_str_11, create_array_obj(arr_5, 4));

    char input_str_12[] = 
        "let reduce = fn(arr, initial, f) { let iter = fn(arr, result) { if (len(arr) == 0) { result } else { iter(rest(arr), f(result, first(arr))); } }; iter(arr, initial); };"
        "let sum = fn(arr) { reduce(arr, 0, fn(initial, el) { initial + el }); };"
        "let a = [1, 2, 3, 4, 5];"
        "sum(a);";
    assert_prog_output(input_str_12, create_int_obj(15));

    // THIS BREAKS THE PARSER
    // char input_str_11[] = 
    //     "let map = fn(arr, f) { let iter = fn(arr, accumulated) { if (len(arr) == 0) { accumulated } else { iter(rest(arr), push(accumulated, f(first(arr)))); }  iter(arr, []); };"
    //     "let a = [1, 2, 3, 4];"
    //     "let double = fn(x) { x * 2 };"
    //     "map(a, double);";
    // int arr_5[] = {2, 4, 6, 8};
    // assert_prog_output(input_str_11, create_array_obj(arr_5, 4));
}


bool test_eval_map()
{
    char input_str_1[] =
        "let map = {0: \"zero\", 1: \"one\", 99: \"ninety nine\"};"
        "map[99]";
    assert_prog_output(input_str_1, create_str_obj("ninety nine"));
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
    test_eval_string();
    test_eval_builtin();
    test_eval_array();
    test_eval_map();
}

#endif
