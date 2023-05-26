#ifndef _EVALC_
#define _EVALC_

#include "../ast/expressions.c"
#include "../parser/statements.c"
#include <assert.h>
#include <string.h>

#define ENV_MAP_SIZE 128

literal eval_program(stmt_list* p);

typedef struct env_map {
    char* key;
    literal value;
} env_map;

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

void insert_env(env_map* map, char* key, literal val) {
    env_map entry = {key, val};
    map[get_hash_env(key)] = entry;
}


literal get_env(env_map* map, char* key) {
    env_map entry = map[get_hash_env(key)];
    if (strcmp(entry.key, key) == 0) {
        return map[get_hash_env(key)].value;
    }
    else {
        printf("ERROR: tried to lookup identifier %s which does not exist.", key);
        literal null;
        null.type = NULL_LIT;
        return null;
    }
}


literal cast_as_bool(literal l) {
    literal b;
    b.type = BOOL_LIT;
    switch (l.type) {
        case BOOL_LIT:
            b.data.b = l.data.b;
            break;
        case INT_LIT:
            b.data.b = l.data.i;
            break;
        case FN_LIT:
            b.data.b = l.data.fn.body.count;
            break;
        case NULL_LIT:
            b.data.b = false;
            break;
        default:
            assert(false);
    }
    return b;
}


literal eval_prefix(token op, literal right) {
    literal null;
    null.type = NULL_LIT;
    switch (op.type) {
        case BANG:
            right = cast_as_bool(right);
            right.data.b = !right.data.b;
            return right;
        case MINUS:
            if (right.type != INT_LIT) {
                return null;
            }
            right.data.i = -right.data.i;
            return right;
        default:
            assert(false);
    }
}


literal lit_gt(literal left, literal right) {
    literal out;
    out.type = NULL_LIT;

    if (left.type != right.type) {
        return out;
    }

    switch (left.type) {
        case INT_LIT:
            out.type = BOOL_LIT;
            out.data.b = left.data.i > right.data.i;
            return out;
        default:
            return out;
    }
}


literal lit_lt(literal left, literal right) {
    literal out;
    out.type = NULL_LIT;

    if (left.type != right.type) {
        return out;
    }

    switch (left.type) {
        case INT_LIT:
            out.type = BOOL_LIT;
            out.data.b = left.data.i < right.data.i;
            return out;
        default:
            return out;
    }
}


literal lit_eq(literal left, literal right) {
    literal out;
    out.type = NULL_LIT;

    if (left.type != right.type) {
        return out;
    }

    switch (left.type) {
        case INT_LIT:
            out.type = BOOL_LIT;
            out.data.b = left.data.i == right.data.i;
            return out;
        default:
            return out;
    }
}


literal lit_neq(literal left, literal right) {
    literal out;
    out.type = NULL_LIT;

    if (left.type != right.type) {
        return out;
    }

    switch (left.type) {
        case INT_LIT:
            out.type = BOOL_LIT;
            out.data.b = left.data.i != right.data.i;
            return out;
        default:
            return out;
    }
}


literal eval_infix(token op, literal left, literal right) {
    literal out;
    out.type = NULL_LIT;


    switch (op.type) {
        case PLUS:
            if (left.type != INT_LIT || right.type != INT_LIT) {
                return out;
            }
            out.type = INT_LIT;
            out.data.i = left.data.i + right.data.i;
            return out;
        case MINUS:
            if (left.type != INT_LIT || right.type != INT_LIT) {
                return out;
            }
            out.type = INT_LIT;
            out.data.i = left.data.i - right.data.i;
            return out;
        case ASTERISK:
            if (left.type != INT_LIT || right.type != INT_LIT) {
                return out;
            }
            out.type = INT_LIT;
            out.data.i = left.data.i * right.data.i;
            return out;
        case SLASH:
            if (left.type != INT_LIT || right.type != INT_LIT) {
                return out;
            }
            out.type = INT_LIT;
            out.data.i = left.data.i / right.data.i;
            return out;
        case GT:
            return lit_gt(left, right);
        case LT:
            return lit_lt(left, right);
        case EQ:
            return lit_eq(left, right);
        case NOT_EQ:
            return lit_neq(left, right);
        default:
            return out;
    }
}


literal eval_expr(expr* e, env_map* env) {
    literal null;
    null.type = NULL_LIT;

    switch (e->type) {
        case LITERAL_EXPR:
            // Resolve any identifier
            if (e->data.lit.type == IDENT_LIT) {
                return get_env(env, e->data.lit.data.t.value);
            }
            return e->data.lit;
        case PREFIX_EXPR:
            return eval_prefix(e->data.pre.operator, eval_expr(e->data.pre.right, env));
        case INFIX_EXPR:
            return eval_infix(e->data.inf.operator, eval_expr(e->data.inf.left, env), eval_expr(e->data.inf.right, env));
        case IF_EXPR:
            if (cast_as_bool(eval_expr(e->data.ifelse.condition, env)).data.b) {
                return eval_program(&e->data.ifelse.consequence);
            } else if (e->data.ifelse.has_alt) {
                return eval_program(&e->data.ifelse.alternative);
            } else {
                return null;
            }
        default:
            return null;
    }
}


literal eval_stmt(stmt* s, env_map* env) {
    literal null;
    null.type = NULL_LIT;

    switch (s->type) {
        // Can I just combine ret into expr
        case RETURN_STMT:
            return eval_expr(s->data.ret.value, env);
        case EXPR_STMT:
            return eval_expr(s->data.expr.value, env);
        case LET_STMT:
            insert_env(env, s->data.let.identifier.value, eval_expr(s->data.let.value, env));
            return null;
        default:
            return null;
    }
}


literal eval_program(stmt_list* p) {
    env_map* env = new_env_map();
    literal out;

    for (int i = 0; i < p->count; i++) {
        out = eval_stmt(&p->statements[i], env);
        if (p->statements[i].type == RETURN_STMT) {
            break;
        }
    }

    free(env);
    return out;
}


#endif