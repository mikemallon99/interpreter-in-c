#ifndef _EVALC_
#define _EVALC_

#include "../ast/expressions.c"
#include "../parser/statements.c"
#include <string.h>

#define ENV_MAP_SIZE 128

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