#include <string.h>

#include "builtins.h"

object _builtin_len(object_list args);
object _string_len(object input_obj);

object call_builtin(object builtin_fn, object_list args) 
{
    switch (builtin_fn.builtin_fn) {
        case BUILTIN_LEN:
            return _builtin_len(args);
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
    switch (input_obj.lit.type) {
        case STRING_LIT:
            return _string_len(input_obj);
        default:
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

