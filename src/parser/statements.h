#ifndef _STATEMENTSH_
#define _STATEMENTSH_

#include <stdlib.h>

#include "../tokens.h"

typedef struct expr expr;

typedef enum
{
    LET_STMT,
    RETURN_STMT,
    EXPR_STMT,
    NULL_STMT
} stmt_type;

typedef struct
{
    token identifier;
    expr* value;
} let_stmt;

typedef struct
{
    expr* value;
} return_stmt;

typedef struct
{
    expr* value;
} expr_stmt;

typedef union
{
    let_stmt let;
    return_stmt ret;
    expr_stmt expr;
} stmt_data;

typedef struct
{
    stmt_type type;
    stmt_data data;
} stmt;

typedef struct stmt_list
{
    stmt* statements;
    size_t count;
    size_t capacity;
} stmt_list;

char* statement_string(stmt* s);
stmt_list new_stmt_list();
void append_stmt_list(stmt_list* cur_list, stmt new_stmt);
char* program_string(stmt_list* p);

#endif