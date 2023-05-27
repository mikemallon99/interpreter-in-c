#include "ast_test.c"
#include "eval_test.c"
#include "lexer_test.c"
#include "parser_test.c"

int main()
{
    run_all_lexer_tests();
    run_all_parser_tests();
    run_all_ast_tests();
    run_all_eval_tests();
}