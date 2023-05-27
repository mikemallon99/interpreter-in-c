#ifndef _EVALH_
#define _EVALH_

#include "../ast/expressions.h"
#include "../parser/statements.h"

typedef enum
{
    LIT_OBJ,
    RET_OBJ,
    ERR_OBJ
} object_type;

typedef struct object
{
    object_type type;
    literal lit;
    char* err;
} object;

typedef struct env_map
{
    char* key;
    object value;
} env_map;

typedef struct environment environment;

typedef struct environment
{
    env_map* inner;
    environment* outer;
} environment;

object eval_program(stmt_list* p, environment* env);
env_map* new_env_map();

#endif