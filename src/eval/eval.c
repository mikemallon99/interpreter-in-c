#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "eval.h"

#define ENV_MAP_SIZE 128

object _create_null_obj();
object _create_lit_obj(literal l);
object _create_err_obj(const char * format, ...);

bool _is_function(object obj);
bool _is_error(object obj);
object _cast_as_bool(object l);

int _get_hash_env(char* str);
void _insert_env(env_map* map, char* key, object val);
object _get_env(env_map* map, char* key);
object _get_literal(environment* env, char* key);

object _lit_gt(object left, object right);
object _lit_lt(object left, object right);
object _lit_eq(object left, object right);
object _lit_neq(object left, object right);
object _eval_prefix(token op, object right);
object _eval_infix(token op, object left, object right);
object _eval_expr(expr* e, environment* env);
object _eval_stmt(stmt* s, environment* env);


// PUBLIC FUNCTIONS

env_map* new_env_map() {
    return (env_map*)calloc(ENV_MAP_SIZE, sizeof(env_map));
}

object eval_program(stmt_list* p, environment* env) {
    object out;

    for (int i = 0; i < p->count; i++) {
        out = _eval_stmt(&p->statements[i], env);
        if (out.type == RET_OBJ || out.type == ERR_OBJ) {
            break;
        }
    }

    return out;
}


// PRIVATE FUNCTIONS

object _create_null_obj() {
    object null;
    null.type = LIT_OBJ;
    null.lit.type = NULL_LIT;
    return null;
}

object _create_lit_obj(literal l) {
    object obj;
    obj.type = LIT_OBJ;
    obj.lit = l;
    return obj;
}

object _create_err_obj(const char * format, ...)
{
    object obj;
    obj.type = ERR_OBJ;

    char* buffer = malloc(256);
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    obj.err = buffer;
    va_end(args);
    return obj;
}

bool _is_error(object obj) {
    return obj.type == ERR_OBJ;
}

bool _is_function(object obj) {
    return obj.lit.type == FN_LIT;
}

// Found a random string hash function
int _get_hash_env(char* str) {
    int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % ENV_MAP_SIZE;
}

void _insert_env(env_map* map, char* key, object val) {
    env_map entry = {key, val};
    map[_get_hash_env(key)] = entry;
}


object _get_env(env_map* map, char* key) {
    env_map entry = map[_get_hash_env(key)];
    if (entry.key == NULL) {
        return _create_err_obj("tried to lookup identifier %s which does not exist", key);
    }
    else if (strcmp(entry.key, key) == 0) {
        return map[_get_hash_env(key)].value;
    }
    else {
        return _create_err_obj("tried to lookup identifier %s but found collision", key);
    }
}


object _get_literal(environment* env, char* key) {
    object val;
    val = _get_env(env->inner, key);
    if (val.type == ERR_OBJ && env->outer != NULL) {
        return _get_literal(env->outer, key);
    }
    return val;
}


object _cast_as_bool(object l) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object b = _create_lit_obj(bool_lit);

    switch (l.lit.type) {
        case BOOL_LIT:
            b.lit.data.b = l.lit.data.b;
            break;
        case INT_LIT:
            b.lit.data.b = l.lit.data.i;
            break;
        case FN_LIT:
            b.lit.data.b = l.lit.data.fn.body.count;
            break;
        case NULL_LIT:
            b.lit.data.b = false;
            break;
        default:
            return _create_err_obj("cannot cast object as bool");
    }
    return b;
}


object _eval_prefix(token op, object right) {
    switch (op.type) {
        case BANG:
            right = _cast_as_bool(right);
            right.lit.data.b = !right.lit.data.b;
            return right;
        case MINUS:
            if (right.lit.type == INT_LIT) {
                right.lit.data.i = -right.lit.data.i;
                return right;
            }
            return _create_err_obj("operator not supported: -");
        default:
            assert(false);
    }
}


object _lit_gt(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i > right.lit.data.i;
            return out;
        default:
            return _create_err_obj("operator not supported: >");
    }
}


object _lit_lt(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i < right.lit.data.i;
            return out;
        default:
            return _create_err_obj("operator not supported: <");
    }
}


object _lit_eq(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i == right.lit.data.i;
            return out;
        default:
            return _create_err_obj("operator not supported: ==");
    }
}


object _lit_neq(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i != right.lit.data.i;
            return out;
        default:
            return _create_err_obj("operator not supported: !=");
    }
}


object _eval_infix(token op, object left, object right) {
    object out;

    if (left.lit.type != right.lit.type) {
        return _create_err_obj("type mismatch: %s != %s", lit_type_string(left.lit), lit_type_string(right.lit));
    }

    switch (op.type) {
        case PLUS:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i + right.lit.data.i;
                return out;
            }
            return _create_err_obj("operator not supported: %s", op.value);
        case MINUS:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i - right.lit.data.i;
                return out;
            }
            return _create_err_obj("operator not supported: %s", op.value);
        case ASTERISK:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i * right.lit.data.i;
                return out;
            }
            return _create_err_obj("operator not supported: %s", op.value);
        case SLASH:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i / right.lit.data.i;
                return out;
            }
            return _create_err_obj("operator not supported: %s", op.value);
        case GT:
            return _lit_gt(left, right);
        case LT:
            return _lit_lt(left, right);
        case EQ:
            return _lit_eq(left, right);
        case NOT_EQ:
            return _lit_neq(left, right);
        default:
            return _create_err_obj("operator not supported: %s", op.value);
    }
}


object _eval_expr(expr* e, environment* env) {
    object out;
    object left, right;
    object func;
    expr_list args;

    switch (e->type) {
        case LITERAL_EXPR:
            // Resolve any identifier
            if (e->data.lit.type == IDENT_LIT) {
                return _get_literal(env, e->data.lit.data.t.value);
            }
            else if (e->data.lit.type == FN_LIT) {
                e->data.lit.data.fn.env->outer = env;
            }
            return _create_lit_obj(e->data.lit);
        case PREFIX_EXPR:
            out = _eval_expr(e->data.pre.right, env);
            if (_is_error(out)) {
                return out;
            }
            return _eval_prefix(e->data.pre.op, out);
        case INFIX_EXPR:
            left = _eval_expr(e->data.inf.left, env);
            if (_is_error(left)) {
                return left;
            }
            right = _eval_expr(e->data.inf.right, env);
            if (_is_error(right)) {
                return right;
            }
            return _eval_infix(e->data.inf.op, left, right);
        case IF_EXPR:
            out = _eval_expr(e->data.ifelse.condition, env);
            if (_is_error(out)) {
                return out;
            }
            out = _cast_as_bool(out);
            if (_is_error(out)) {
                return out;
            }
            if (out.lit.data.b) {
                return eval_program(&e->data.ifelse.consequence, env);
            } else if (e->data.ifelse.has_alt) {
                return eval_program(&e->data.ifelse.alternative, env);
            } else {
                return _create_null_obj();
            }
        case CALL_EXPR:
            func = _eval_expr(e->data.call.func, env);
            if (_is_error(func)) {
                return func;
            }
            
            args = e->data.call.args;
            if (args.count != func.lit.data.fn.params.count) {
                _create_err_obj("arg mismatch, input: %d fn: %d", args.count, func.lit.data.fn.params.count);
            }

            func.lit.data.fn.env->inner = new_env_map();
            for (int i = 0; i < args.count; i++) {
                out = _eval_expr(args.exprs[i], env);
                if (_is_error(out)) {
                    return out;
                }
                _insert_env(func.lit.data.fn.env->inner, func.lit.data.fn.params.tokens[i].value, out);
            }
            return eval_program(&func.lit.data.fn.body, func.lit.data.fn.env);
        default:
            return _create_null_obj();
    }
}


object _eval_stmt(stmt* s, environment* env) {
    object out;

    switch (s->type) {
        // Can I just combine ret into expr
        case RETURN_STMT:
            out = _eval_expr(s->data.ret.value, env);
            if (_is_error(out)) {
                return out;
            }
            out.type = RET_OBJ;
            return out;
        case EXPR_STMT:
            return _eval_expr(s->data.expr.value, env);
        case LET_STMT:
            out = _eval_expr(s->data.let.value, env);
            if (_is_error(out)) {
                return out;
            } 
            _insert_env(env->inner, s->data.let.identifier.value, out);
            return _create_null_obj();
        default:
            return _create_null_obj();
    }
}