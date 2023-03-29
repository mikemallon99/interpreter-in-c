#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.c"

bool test_let_statement(statement s, char* name) {
    if (s.type != SMT_LET) {
        printf("Identifier is not a let statement\n");
        return false;
    }

    smt_let_data* data = (smt_let_data*) s.data;
    if (data->t.tokenType != LET) {
        printf("Statement is not a LET token\n");
        return false;
    }
    if (data->ident.tokenType != IDENT) {
        printf("Identifier is not an IDENT token\n");
        return false;
    }
    if (strcmp(data->ident.value, name) != 0) {
        printf("Discovered ident name doesn't equal expected name: '%s' != '%s'\n", data->ident.value, name);
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

    program* pgm = parse_program(&p);
    assert(check_parser_errors(&p));
    if (pgm == NULL) {
        printf("parse_program returned NULL");
        return;
    }
    if (pgm->num_statements != 3) {
        printf("parse_program did not collect 3 statements. Got %d", pgm->num_statements);
        return;
    }

    statement cur_statement;

    // Assign cur statement to next item in linked list
    test_let_statement((cur_statement = *(pgm->statements)), "x");
    test_let_statement((cur_statement = *(cur_statement.next)), "y");
    test_let_statement((cur_statement = *(cur_statement.next)), "foobar");
}

int main() {
    test_let_statements();
    test_bad_let_statements();
    return 0;
}
