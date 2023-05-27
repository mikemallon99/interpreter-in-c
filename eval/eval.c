#ifndef _EVALC_
#define _EVALC_

#include "../ast/expressions.c"
#include "../parser/statements.c"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define ENV_MAP_SIZE 128


typedef enum {
    LIT_OBJ, RET_OBJ, ERR_OBJ
} object_type;

typedef struct object {
    object_type type;
    literal lit;
    char* err;
} object;

object create_null_obj() {
    object null;
    null.type = LIT_OBJ;
    null.lit.type = NULL_LIT;
    return null;
}

object create_lit_obj(literal l) {
    object obj;
    obj.type = LIT_OBJ;
    obj.lit = l;
    return obj;
}

object create_err_obj(const char * format, ...)
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

typedef struct env_map {
    char* key;
    object value;
} env_map;

object eval_program(stmt_list* p, env_map* env);

env_map* new_env_map() {
    return (env_map*)malloc(sizeof(env_map) * ENV_MAP_SIZE);
}

// Found a random string hash function
int get_hash_env(char* str) {
    int hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % ENV_MAP_SIZE;
}

void insert_env(env_map* map, char* key, object val) {
    env_map entry = {key, val};
    map[get_hash_env(key)] = entry;
}


object get_env(env_map* map, char* key) {
    env_map entry = map[get_hash_env(key)];
    if (strcmp(entry.key, key) == 0) {
        return map[get_hash_env(key)].value;
    }
    else {
        printf("ERROR: tried to lookup identifier %s which does not exist.", key);
        return create_null_obj();
    }
}


object cast_as_bool(object l) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object b = create_lit_obj(bool_lit);

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
            return create_err_obj("cannot cast object as bool");
    }
    return b;
}


object eval_prefix(token op, object right) {
    switch (op.type) {
        case BANG:
            right = cast_as_bool(right);
            right.lit.data.b = !right.lit.data.b;
            return right;
        case MINUS:
            if (right.lit.type == INT_LIT) {
                right.lit.data.i = -right.lit.data.i;
                return right;
            }
            return create_err_obj("operator not supported: -");
        default:
            assert(false);
    }
}


object lit_gt(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i > right.lit.data.i;
            return out;
        default:
            return create_err_obj("operator not supported: >");
    }
}


object lit_lt(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i < right.lit.data.i;
            return out;
        default:
            return create_err_obj("operator not supported: <");
    }
}


object lit_eq(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i == right.lit.data.i;
            return out;
        default:
            return create_err_obj("operator not supported: ==");
    }
}


object lit_neq(object left, object right) {
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type) {
        return out;
    }

    switch (left.lit.type) {
        case INT_LIT:
            out.lit.type = BOOL_LIT;
            out.lit.data.b = left.lit.data.i != right.lit.data.i;
            return out;
        default:
            return create_err_obj("operator not supported: !=");
    }
}


object eval_infix(token op, object left, object right) {
    object out;

    if (left.lit.type != right.lit.type) {
        return create_err_obj("type mismatch: %s != %s", lit_type_string(left.lit), lit_type_string(right.lit));
    }

    switch (op.type) {
        case PLUS:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i + right.lit.data.i;
                return out;
            }
            return create_err_obj("operator not supported: %s", op.value);
        case MINUS:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i - right.lit.data.i;
                return out;
            }
            return create_err_obj("operator not supported: %s", op.value);
        case ASTERISK:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i * right.lit.data.i;
                return out;
            }
            return create_err_obj("operator not supported: %s", op.value);
        case SLASH:
            if (left.lit.type == INT_LIT) {
                out.lit.type = INT_LIT;
                out.lit.data.i = left.lit.data.i / right.lit.data.i;
                return out;
            }
            return create_err_obj("operator not supported: %s", op.value);
        case GT:
            return lit_gt(left, right);
        case LT:
            return lit_lt(left, right);
        case EQ:
            return lit_eq(left, right);
        case NOT_EQ:
            return lit_neq(left, right);
        default:
            return create_err_obj("operator not supported: %s", op.value);
    }
}


object eval_expr(expr* e, env_map* env) {
    switch (e->type) {
        case LITERAL_EXPR:
            // Resolve any identifier
            if (e->data.lit.type == IDENT_LIT) {
                return get_env(env, e->data.lit.data.t.value);
            }
            return create_lit_obj(e->data.lit);
        case PREFIX_EXPR:
            return eval_prefix(e->data.pre.operator, eval_expr(e->data.pre.right, env));
        case INFIX_EXPR:
            return eval_infix(e->data.inf.operator, eval_expr(e->data.inf.left, env), eval_expr(e->data.inf.right, env));
        case IF_EXPR:
            if (cast_as_bool(eval_expr(e->data.ifelse.condition, env)).lit.data.b) {
                return eval_program(&e->data.ifelse.consequence, env);
            } else if (e->data.ifelse.has_alt) {
                return eval_program(&e->data.ifelse.alternative, env);
            } else {
                return create_null_obj();
            }
        default:
            return create_null_obj();
    }
}


object eval_stmt(stmt* s, env_map* env) {
    object out;

    switch (s->type) {
        // Can I just combine ret into expr
        case RETURN_STMT:
            out = eval_expr(s->data.ret.value, env);
            return out;
        case EXPR_STMT:
            return eval_expr(s->data.expr.value, env);
        case LET_STMT:
            insert_env(env, s->data.let.identifier.value, eval_expr(s->data.let.value, env));
            return create_null_obj();
        default:
            return create_null_obj();
    }
}


object eval_program(stmt_list* p, env_map* env) {
    object out;

    for (int i = 0; i < p->count; i++) {
        out = eval_stmt(&p->statements[i], env);
        if (p->statements[i].type == RETURN_STMT) {
            out.type = RET_OBJ;
            break;
        } else if (out.type == RET_OBJ || out.type == ERR_OBJ) {
            break;
        }
    }

    return out;
}


#endif