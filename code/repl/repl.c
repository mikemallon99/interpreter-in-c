#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../lexer/lexer.c"

void start() {
    char input[100];
    while (true) {
        printf(">> ");
        fgets(input, 100, stdin);

        if (strcmp(input, "") == 0) {
            return;
        }

        lexer l = get_lexer(input);
        for (token t = next_token(&l); t.tokenType != EOF_T; t = next_token(&l)) {
            char* type_string = calloc(10, 1);
            strcpy(type_string, get_token_type_string(t.tokenType));
            printf("{Type: %s, Value: '%s'}\n", type_string, t.value);
            free(type_string);
            free(t.value);
        }
    }

}

