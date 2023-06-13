#ifndef _EVALH_
#define _EVALH_

#include "../ast/expressions.h"
#include "../parser/statements.h"

typedef enum
{
    LIT_OBJ,
    ARRAY_OBJ,
    BUILTIN_OBJ,
    RET_OBJ,
    ERR_OBJ,
    NULL_OBJ
} object_type;

typedef struct object object;

typedef struct object_list
{
    object* objs;
    size_t count;
    size_t capacity;
} object_list;

typedef enum
{
    BUILTIN_LEN,
    BUILTIN_FIRST,
    BUILTIN_LAST,
    BUILTIN_REST,
    BUILTIN_PUSH
} builtin_name;

typedef struct object
{
    object_type type;
    literal lit;
    char* err;
    builtin_name builtin_fn;
    object_list arr;
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
object copy_object(object obj);
void cleanup_object(object obj);
void cleanup_stmt_list(stmt_list st_lst);

char* object_string(object obj);

object_list new_object_list();
void append_object_list(object_list* cur_list, object new_object);
char* object_list_string(object_list* cur_list);


#endif