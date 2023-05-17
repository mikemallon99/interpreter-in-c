#include "../parser/parser.c"
#include <assert.h>
#include "eval.c"

bool test_eval_int() {
    char input_str[] = "5;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 1);

    literal out = eval_program(&prog);

    printf("test_eval_int: %s\n", literal_string(out));
    assert(out.type == INT_LIT);
    assert(out.data.i == 5);

    return true;
}

bool test_eval_let() {
    char input_str[] = "let five = 5; five;";
    lexer l = get_lexer(input_str);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    assert(prog.count == 2);

    literal out = eval_program(&prog);

    printf("test_eval_let: %s\n", literal_string(out));
    assert(out.type == INT_LIT);
    assert(out.data.i == 5);

    return true;
}

int main() {
    test_eval_int();
    test_eval_let();
    return 0;
}