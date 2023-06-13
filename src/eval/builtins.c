#include <string.h>
#include <stdio.h>

#include "builtins.h"

object _builtin_len(object_list args);
object _string_len(object input_obj);

object _builtin_first(object_list args);
object _builtin_last(object_list args);
object _builtin_rest(object_list args);
object _builtin_push(object_list args);
object _builtin_print(object_list args);

object call_builtin(object builtin_fn, object_list args) 
{
    switch (builtin_fn.builtin_fn) {
        case BUILTIN_LEN:
            return _builtin_len(args);
        case BUILTIN_FIRST:
            return _builtin_first(args);
        case BUILTIN_LAST:
            return _builtin_last(args);
        case BUILTIN_REST:
            return _builtin_rest(args);
        case BUILTIN_PUSH:
            return _builtin_push(args);
        case BUILTIN_PRINT:
            return _builtin_print(args);
        default:
            return create_err_obj("Cannot find builtin function.");
    }
}

object _builtin_len(object_list args) 
{
    if (args.count > 1) {
        return create_err_obj("len(): too many args");
    }
    object input_obj = args.objs[0];
    if (input_obj.type == LIT_OBJ) {
        switch (input_obj.lit.type) {
            case STRING_LIT:
                return _string_len(input_obj);
            default:
                return create_err_obj("Cannot find len() builtin for given type.");
        }
    }
    else if (input_obj.type == ARRAY_OBJ) {
        literal_data lit_data;
        lit_data.i = input_obj.arr.count;
        literal out = {INT_LIT, lit_data};

        return create_lit_obj(out);
    }
    else {
        return create_err_obj("Cannot find len() builtin for given type.");
    }
}

object _string_len(object input_obj) 
{
    literal_data lit_data;
    lit_data.i = strlen(input_obj.lit.data.s);
    literal out = {INT_LIT, lit_data};

    return create_lit_obj(out);
}


object _builtin_first(object_list args) 
{
    if (args.count > 1) {
        return create_err_obj("first(): too many args");
    }
    object input_obj = args.objs[0];
    if (input_obj.type != ARRAY_OBJ) {
        return create_err_obj("Cannot find first() builtin for given type.");
    }

    if (input_obj.arr.count > 0) {
        return input_obj.arr.objs[0];
    }
    else {
        return create_err_obj("Cannot call first() on an empty array.");
    }
}

object _builtin_last(object_list args) 
{
    if (args.count > 1) {
        return create_err_obj("last(): too many args");
    }
    object input_obj = args.objs[0];
    if (input_obj.type != ARRAY_OBJ) {
        return create_err_obj("Cannot find last() builtin for given type.");
    }

    if (input_obj.arr.count > 0) {
        return input_obj.arr.objs[input_obj.arr.count-1];
    }
    else {
        return create_err_obj("Cannot call last() on an empty array.");
    }
}

object _builtin_rest(object_list args) 
{
    if (args.count > 1) {
        return create_err_obj("rest(): too many args");
    }
    object input_obj = args.objs[0];
    if (input_obj.type != ARRAY_OBJ) {
        return create_err_obj("Cannot find rest() builtin for given type.");
    }

    if (input_obj.arr.count > 0) {
        object out_arr;
        out_arr.type = ARRAY_OBJ;
        out_arr.arr = new_object_list();
        for (int i = 1; i < input_obj.arr.count; i++) {
            append_object_list(&out_arr.arr, copy_object(input_obj.arr.objs[i]));
        }
        return out_arr;
    }
    else {
        return create_err_obj("Cannot call rest() on an empty array.");
    }
}

object _builtin_push(object_list args) 
{
    if (args.count < 2) {
        return create_err_obj("push(): too few args");
    }
    if (args.count > 2) {
        return create_err_obj("push(): too many args");
    }

    object arr_obj = args.objs[0];
    if (arr_obj.type != ARRAY_OBJ) {
        return create_err_obj("push(): arg1 must be an array");
    }
    object push_obj = args.objs[1];

    object out_arr;
    out_arr.type = ARRAY_OBJ;
    out_arr.arr = new_object_list();
    for (int i = 0; i < arr_obj.arr.count; i++) {
        append_object_list(&out_arr.arr, copy_object(arr_obj.arr.objs[i]));
    }
    append_object_list(&out_arr.arr, copy_object(push_obj));

    return out_arr;
}

object _builtin_print(object_list args) 
{
    if (args.count > 1) {
        return create_err_obj("print(): too many args");
    }

    object input_obj = args.objs[0];
    char* obj_str = object_string(input_obj);
    printf("%s\n", obj_str);

    return create_null_obj();
}