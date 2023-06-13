#include <stdlib.h>
// #include <crtdbg.h>
#include <stdio.h>
#include <string.h>

#include "repl.h"
#include "../parser/parser.h"
#include "../eval/eval.h"


void execute_file(char* file_contents)
{
    lexer l = get_lexer(file_contents);
    parser p = new_parser(&l);
    stmt_list prog = parse_program(&p);
    if (!check_parser_errors(&p)) {
        return;
    }

    environment env;
    env_map inner = new_env_map();
    env.inner = inner;
    env.outer = NULL;
    object out = eval_program(&prog, &env);
    char* out_string = object_string(out);
    printf("%s\n", out_string);
    cleanup_object(out);
    force_cleanup_environment(&env);
}


void start()
{
    char input[1024];
    lexer l;
    parser p;
    stmt_list prog;
    object out;
    environment env;
    env_map inner = new_env_map();
    env.inner = inner;
    env.outer = NULL;


    while (true)
    {
        printf(">> ");
        fgets(input, 1024, stdin);

        if (strcmp(input, "") == 0)
        {
            return;
        }

        l = get_lexer(input);
        p = new_parser(&l);
        prog = parse_program(&p);
        if (!check_parser_errors(&p)) {
            continue;
        }

        out = eval_program(&prog, &env);
        char* out_string = object_string(out);
        printf("%s\n", out_string);
        cleanup_object(out);
    }

    force_cleanup_environment(&env);
}
