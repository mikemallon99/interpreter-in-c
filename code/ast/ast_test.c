#include <assert.h>
#include <stdbool.h>
#include "../lexer/lexer.c"
#include "../parser/parser.c"
#include "../ast/ast_string.c"

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

int main() {
    assert(test_program_string());
    return 0;
}
