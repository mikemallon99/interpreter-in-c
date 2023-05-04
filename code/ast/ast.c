#ifndef _ASTC_
#define _ASTC_

#include <stdlib.h>
#include <string.h>
#include "../tokens.c"
#include "../parser/parser.c"

// Expression data types
//
char* expression_string(expression* e) {
    char* expr_str = malloc(128);
    strcpy(expr_str, "");
    return expr_str;
}

char* statement_string(stmt* s) {
    char* expr_str;
    char* stmt_str = malloc(128);

    switch (s->type) {
        case LET_STMT:
            expr_str = expression_string(s->data.let.value);
            sprintf(stmt_str, "let %s = %s;\n", s->data.let.identifier.value, expr_str);
            free(expr_str);
            break;
        case RETURN_STMT:
            expr_str = expression_string(s->data.ret.value);
            sprintf(stmt_str, "return %s;\n", expr_str);
            free(expr_str);
            break;
        case EXPR_STMT:
            expr_str = expression_string(s->data.expr.value);
            sprintf(stmt_str, "%s;\n", expr_str);
            free(expr_str);
            break;
        case NULL_STMT:
            sprintf(stmt_str, "NULL Expression\n");
            break;
    }

    return stmt_str;
}

char* program_string(stmt_list* p) {
    char* stmt_str = malloc(2048);
    char* cur_str; 
    int str_pos = 0;

    // Just tack together all the statement strings
    for (int i = 0; i < p->count; i++) {
        cur_str = statement_string(&p->statements[i]);
        strcpy(stmt_str + str_pos, cur_str);
        str_pos += strlen(cur_str);
    }
    
    return stmt_str;
}

#endif
