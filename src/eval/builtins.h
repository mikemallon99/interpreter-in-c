#ifndef _BUILTINSH_
#define _BUILTINSH_

#include "eval.h"

typedef enum
{
    BUILTIN_LEN
} builtin_name;

typedef struct object_list
{
    object* objs;
    size_t count;
    size_t capacity;
} object_list;

object_list new_object_list();
void append_object_list(object_list* cur_list, object new_object);

object call_builtin(object builtin_fn, object_list args);

#endif