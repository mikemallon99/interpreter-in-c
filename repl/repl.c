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
    object out;
    environment env;
    env_map* inner = new_env_map();
    env.inner = inner;

    while (true) {
        printf(">> ");
        fgets(input, 100, stdin);

        if (strcmp(input, "") == 0) {
            return;
        }

        l = get_lexer(input);
        p = new_parser(&l);
        prog = parse_program(&p);
        out = eval_program(&prog, &env);
        if (out.type == ERR_OBJ) {
            printf("%s\n", out.err);
        } else {
            printf("%s\n", literal_string(out.lit));
        }
    }

    free(env.inner);
}


#endif
