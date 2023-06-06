#ifndef _EVALH_
#define _EVALH_

#include "../ast/expressions.h"
#include "../parser/statements.h"

typedef enum
{
    LIT_OBJ,
    BUILTIN_OBJ,
    RET_OBJ,
    ERR_OBJ
} object_type;

typedef struct object
{
    object_type type;
    literal lit;
    char* err;
    unsigned int builtin_fn;
} object;

typedef struct env_map_entry
{
    char* key;
    object value;
} env_map_entry;

typedef struct env_map
{
    env_map_entry* entries;
    int ref_count;
} env_map;

typedef struct environment environment;

typedef struct environment
{
    env_map inner;
    environment* outer;
} environment;

object eval_program(stmt_list* p, environment* env);
env_map new_env_map();

object create_err_obj(const char* format, ...);
object create_lit_obj(literal l);

void cleanup_environment(environment* env);
void force_cleanup_environment(environment* env);
void cleanup_object(object obj);
void cleanup_stmt_list(stmt_list st_lst);

#endif