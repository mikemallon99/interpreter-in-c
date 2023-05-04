#ifndef _STATEMENTSC_
#define _STATEMENTSC_

#include <stdlib.h>
#include "../tokens.c"
#include "expressions.c"

typedef enum statement_type {
    LET_STMT, RETURN_STMT, EXPR_STMT, NULL_STMT
} stmt_type;

typedef struct {
    token identifier;
    expression* value;
} let_stmt;

typedef struct {
    expression* value;
} return_stmt;

typedef struct {
    expression* value;
} expr_stmt;

typedef union {
    let_stmt let;
    return_stmt ret;
    expr_stmt expr;
} stmt_data;

typedef struct {
    stmt_type type;
    stmt_data data;
} stmt;

typedef struct {
    stmt* statements;
    size_t count;
    size_t capacity;
} stmt_list;

stmt_list new_stmt_list() {
    stmt_list new_list;
    new_list.capacity = sizeof(stmt);
    new_list.statements = (stmt*)malloc(new_list.capacity);
    new_list.count = 0;
    return new_list;
}

void append_stmt_list(stmt_list* cur_list, stmt new_stmt) {
    if ((cur_list->count) >= cur_list->capacity) {
        cur_list->statements = (stmt*)realloc(cur_list->statements, cur_list->capacity * 2);
        cur_list->capacity *= 2;
    }
    cur_list->statements[cur_list->count] = new_stmt;
    cur_list->count++;
}

#endif
