#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include "parser.c"

bool test_let_statement(statement s, char* name) {
    if (s.type != SMT_LET) {
        printf("Identifier is not a let statement");
        return false;
    }

    smt_let_data* data = (smt_let_data*) s.data;
    if (data->t.tokenType != LET) {
        printf("Statement is not a LET token");
        return false;
    }
    if (data->ident.tokenType != IDENT) {
        printf("Identifier is not an IDENT token");
        return false;
    }
    if (strcmp(data->ident.value, name) != 0) {
        printf("Discovered ident name doesn't equal expected name: '%s' != '%s'", data->ident.value, name);
        return false;
    }
    return true;
}

void test_let_statements() {
    char input_string[] = 
        "let x = 5;\n"
        "let y = 10;\n"
        "let foobar = 838383;\n";

    lexer l = get_lexer(input_string);
    parser p = new_parser(&l);

    program* pgm = parse_program(&p);
    if (pgm == NULL) {
        printf("parse_program returned NULL");
        return;
    }
    if (pgm->num_statements != 3) {
        printf("parse_program did not collect 3 statements. Got %d", pgm->num_statements);
        return;
    }

    assert(test_let_statement(pgm->statements[0], "x"));
}

int main() {
    test_let_statements();
    return 0;
}
