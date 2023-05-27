#ifndef _EVALH_
#define _EVALH_

#include "../ast/expressions.h"
#include "../parser/statements.h"

typedef enum {
    LIT_OBJ, RET_OBJ, ERR_OBJ
} object_type;

typedef struct object {
    object_type type;
    literal lit;
    char* err;
} object;

typedef struct env_map {
    char* key;
    object value;
} env_map;

typedef struct environment environment;

typedef struct environment {
    env_map* inner;
    environment* outer;
} environment;


object create_null_obj();
object create_lit_obj(literal l);
object create_err_obj(const char * format, ...);
bool is_error(object obj);
bool is_function(object obj);
env_map* new_env_map();
int get_hash_env(char* str);
void insert_env(env_map* map, char* key, object val);
object get_env(env_map* map, char* key);
object get_literal(environment* env, char* key);
object cast_as_bool(object l);
object eval_prefix(token op, object right);
object lit_gt(object left, object right);
object lit_lt(object left, object right);
object lit_eq(object left, object right);
object lit_neq(object left, object right);
object eval_infix(token op, object left, object right);
object eval_expr(expr* e, environment* env);
object eval_stmt(stmt* s, environment* env);
object eval_program(stmt_list* p, environment* env);

#endif