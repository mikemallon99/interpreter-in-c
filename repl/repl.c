#ifndef _REPLC_
#define _REPLC_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../parser/parser.c"
#include "../eval/eval.c"

void start() {
    char input[100];
    lexer l;
    parser p;
    stmt_list prog;
    literal out;
    env_map* env;

    while (true) {
        printf(">> ");
        fgets(input, 100, stdin);

        if (strcmp(input, "") == 0) {
            return;
        }

        l = get_lexer(input);
        p = new_parser(&l);
        prog = parse_program(&p);
        env = new_env_map();
        out = eval_program(&prog, env);
        free(env);
        printf("%s\n", literal_string(out));
    }

}


#endif
