#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.c"
#include "../ast/ast.c"

bool test_let_statement(stmt s, char* name) {
    if (s.type != LET_STMT) {
        printf("Identifier is not a let statement\n");
        return false;
    }

    let_stmt let = s.data.let;
    if (let.identifier.tokenType != IDENT) {
        printf("Identifier is not an IDENT token\n");
        return false;
    }
    if (strcmp(let.identifier.value, name) != 0) {
        printf("Discovered ident name doesn't equal expected name: '%s' != '%s'\n", let.identifier.value, name);
        return false;
    }
    return true;
}

void test_bad_let_statements() {
    char input_string[] = 
        "let x 5;\n"
        "let = 10;\n"
        "let 838383 = x;\n";

    lexer l = get_lexer(input_string);
    parser p = new_parser(&l);

    parse_program(&p);
    // Make sure we got some errors up in here
    assert(!check_parser_errors(&p));
}

void test_let_statements() {
    char input_string[] = 
        "let x = 5;\n"
        "let y = 10;\n"
        "let foobar = 838383;\n";

    lexer l = get_lexer(input_string);
    parser p = new_parser(&l);

    stmt_list prog = parse_program(&p);
    assert(check_parser_errors(&p));
    if (prog.count == 0) {
        printf("parse_program has no statements");
        return;
    }
    if (prog.count != 3) {
        printf("parse_program did not collect 3 statements. Got %lld", prog.count);
        return;
    }

    // Assign cur statement to next item in linked list
    test_let_statement(prog.statements[0], "x");
    test_let_statement(prog.statements[1], "y");
    test_let_statement(prog.statements[2], "foobar");
}

void test_return_statements() {
    char input_string[] = 
        "return 5;\n"
        "return 10;\n"
        "return 993322;\n";

    lexer l = get_lexer(input_string);
    parser p = new_parser(&l);

    stmt_list prog = parse_program(&p);
    assert(check_parser_errors(&p));
    if (prog.count == 0) {
        printf("parse_program has no statements");
        return;
    }
    if (prog.count != 3) {
        printf("parse_program did not collect 3 statements. Got %lld", prog.count);
        return;
    }

}

void test_program_string() {
    // Create let statement from raw tokens
    expression expr_data; 
    let_stmt let = {{IDENT, "my_var"}, &expr_data};
    stmt let_stmt;
    let_stmt.type = LET_STMT;
    let_stmt.data.let = let;
    stmt_list prog;
    append_stmt_list(&prog, let_stmt);
    if (prog.count == 0) {
        printf("parse_program has no statements");
        return;
    }

    char* prog_string = program_string(&prog);
    printf(prog_string);
    free(prog_string);
    printf("hi");
}

int main() {
    test_let_statements();
    test_bad_let_statements();
    test_return_statements();
    test_program_string();
    return 0;
}
