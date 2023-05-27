#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "statements.h"

// STATEMENT DATATYPES & FUNCTIONS
char* statement_string(stmt* s) {
    char* expr_str;
    char* stmt_str = malloc(128);

    switch (s->type) {
        case LET_STMT:
            expr_str = expression_string(s->data.let.value);
            sprintf(stmt_str, "let %s = %s;", s->data.let.identifier.value, expr_str);
            free(expr_str);
            break;
        case RETURN_STMT:
            expr_str = expression_string(s->data.ret.value);
            sprintf(stmt_str, "return %s;", expr_str);
            free(expr_str);
            break;
        case EXPR_STMT:
            expr_str = expression_string(s->data.expr.value);
            sprintf(stmt_str, "%s", expr_str);
            free(expr_str);
            break;
        case NULL_STMT:
            sprintf(stmt_str, "NULL Expression");
            break;
    }

    return stmt_str;
}


// STATEMENT DYNAMIC ARRAY
stmt_list new_stmt_list() {
    stmt_list new_list;
    new_list.capacity = 1;
    new_list.statements = (stmt*)malloc(new_list.capacity * sizeof(stmt));
    new_list.count = 0;
    return new_list;
}

void append_stmt_list(stmt_list* cur_list, stmt new_stmt) {
    if ((cur_list->count) >= cur_list->capacity) {
        cur_list->statements = (stmt*)realloc(cur_list->statements, cur_list->capacity * 2 * sizeof(stmt));
        cur_list->capacity *= 2;
    }
    cur_list->statements[cur_list->count] = new_stmt;
    cur_list->count++;
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
