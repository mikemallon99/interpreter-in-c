#include <stdio.h>
#include "../lexer/lexer.c"

void start() {
    lexer l;
    char input[100];
    while (true) {
        printf(">> ");
        fgets(input, 100, stdin);
    }
}

