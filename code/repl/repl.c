#ifndef _REPLC_
#define _REPLC_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../parser/parser.c"

void start() {
    char input[100];
    lexer l;
    parser p;
    stmt_list prog;

    while (true) {
        printf(">> ");
        fgets(input, 100, stdin);

        if (strcmp(input, "") == 0) {
            return;
        }

        l = get_lexer(input);
        p = new_parser(&l);
        prog = parse_program(&p);
        printf("%s\n", program_string(&prog));
    }

}


#endif
