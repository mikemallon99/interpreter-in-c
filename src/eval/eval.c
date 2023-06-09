#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "eval.h"
#include "builtins.h"

#define ENV_MAP_SIZE 128

bool _is_function(object obj);
bool _is_error(object obj);
object _cast_as_bool(object l);

bool _is_literal_equal(literal lit1, literal lit2);

unsigned long _get_hash_env(char* str);
unsigned long _get_hash_str(char* str, int size);
int _mod(int a, int b);
unsigned long _get_hash_int(int i, int size);
void _insert_env(env_map map, char* key, object val);
object _get_env(env_map map, char* key);
object _eval_identifier(environment* env, char* key);

object _lit_gt(object left, object right);
object _lit_lt(object left, object right);
object _lit_eq(object left, object right);
object _lit_neq(object left, object right);
object _eval_prefix(token op, object right);
object _eval_infix(token op, object left, object right);
object _eval_expr(expr* e, environment* env);
object _eval_stmt(stmt* s, environment* env);
object _eval_array(environment* env, expr_list ex_arr);
object _eval_map(environment* env, expr_pair_list ex_pairs);
object _index_array(object left, object right);
object _index_map(object left, object right);

void _cleanup_token(token tok);
void _cleanup_token_list(token_list tok_lst);
void _cleanup_expr(expr* ex);
void _cleanup_expr_list(expr_list ex_lst);
void _cleanup_stmt(stmt st);
void _cleanup_literal(literal lit);
void _cleanup_env_map(env_map env);

token _copy_token(token tok);
token_list _copy_token_list(token_list tok_lst);
expr* _copy_expr(expr* ex);
expr_list _copy_expr_list(expr_list ex_lst);
stmt _copy_stmt(stmt st);
stmt_list _copy_stmt_list(stmt_list st_lst);
literal _copy_literal(literal lit);
env_map _copy_env_map(env_map env);
environment* _copy_environment(environment* env);

void _increment_env_refs(environment* env);

// PUBLIC FUNCTIONS

bool _is_literal_equal(literal lit1, literal lit2)
{
    if (lit1.type != lit2.type) {
        return false;
    }
    switch (lit1.type)
    {
        case INT_LIT:
            return lit1.data.i == lit2.data.i;
        case BOOL_LIT:
            return lit1.data.b == lit2.data.b;
        case STRING_LIT:
            return strcmp(lit1.data.s, lit2.data.s) == 0;
        case NULL_LIT:
            return true;
        default:
            assert(false);
    }
}

bool is_object_equal(object obj1, object obj2)
{
    if (obj1.type != obj2.type) {
        return false;
    }
    if (obj1.type == LIT_OBJ) {
        return _is_literal_equal(obj1.lit, obj2.lit);
    }
    else if (obj1.type == ARRAY_OBJ) {
        bool is_equal = true;
        for (int i = 0; i < obj1.arr.count; i++) {
            is_equal = is_equal && is_object_equal(obj1.arr.objs[i], obj2.arr.objs[i]);
        }
        return is_equal;
    }
}

object_map new_object_map()
{
    object_map map;
    map.entries = (object_map_entry* )calloc(OBJ_MAP_CAPACITY, sizeof(object_map_entry));
    for (int i = 0; i < OBJ_MAP_CAPACITY; i++) {
        map.entries[i].key.type = NULL_OBJ;
    }
    return map;
}

object lookup_obj_map(object_map map, object key)
{
    int hash_val = get_obj_hash(key);
    if (hash_val < 0) {
        return create_err_obj("Cannot create hash for input type.");
    }
    else {
        if (is_object_equal(key, map.entries[hash_val].key)) {
            return map.entries[hash_val].val;
        }
        else {
            return create_err_obj("Key not found in map.");
        }
    }
}

void insert_obj_map(object_map map, object key, object val)
{
    int hash_val = get_obj_hash(key);
    if (hash_val < 0) {
        // return create_err_obj("Cannot create hash for input type.");
        assert(false);
        return;
    }
    else {
        if (map.entries[hash_val].key.type == NULL_OBJ) {
            map.entries[hash_val].key = key;
            map.entries[hash_val].val = val;
            return;
        }
        else {
            assert(false);
            return;
        }
    }
}

int get_obj_hash(object key)
{
    if (key.type == LIT_OBJ) {
        switch (key.lit.type) {
            case STRING_LIT:
                return _get_hash_str(key.lit.data.s, OBJ_MAP_CAPACITY);
            case INT_LIT:
                return _get_hash_int(key.lit.data.i, OBJ_MAP_CAPACITY);
            case BOOL_LIT:
                return _get_hash_int(key.lit.data.b, OBJ_MAP_CAPACITY);
            default:
                return -1;
        }
    }
}

env_map new_env_map()
{
    env_map env;
    env.entries = (env_map_entry* )calloc(ENV_MAP_SIZE, sizeof(env_map_entry));
    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        env.entries[i].key = NULL;
    }
    env.ref_count = 1;
    return env;
}

object eval_program(stmt_list* p, environment* env)
{
    object out;
    out.type = NULL_OBJ;

    for (int i = 0; i < p->count; i++)
    {
        out = _eval_stmt(&p->statements[i], env);
        if (out.is_return || out.type == ERR_OBJ)
        {
            break;
        }
        if (i != p->count - 1) {
            cleanup_object(out);
        }
    }

    return out;
}


object_list new_object_list()
{
    object_list new_list;
    new_list.capacity = 1;
    new_list.objs = (object*)malloc(new_list.capacity * sizeof(object));
    new_list.count = 0;
    return new_list;
}

void append_object_list(object_list* cur_list, object new_object)
{
    if ((cur_list->count) >= cur_list->capacity)
    {
        cur_list->objs = (object*)realloc(cur_list->objs, cur_list->capacity * 2 * sizeof(object));
        cur_list->capacity *= 2;
    }
    cur_list->objs[cur_list->count] = new_object;
    cur_list->count++;
}


char* object_string(object obj)
{
    char* obj_str = (char*)calloc(256, 1);
    char* temp_str;

    switch(obj.type) {
        case ERR_OBJ:
            strcpy(obj_str, obj.err);
            break;
        case ARRAY_OBJ:
            temp_str = object_list_string(&obj.arr);
            strcpy(obj_str, temp_str);
            free(temp_str);
            break;
        case MAP_OBJ:
            temp_str = object_map_string(obj.map);
            strcpy(obj_str, temp_str);
            free(temp_str);
            break;
        case LIT_OBJ:
            temp_str = literal_string(obj.lit);
            strcpy(obj_str, temp_str);
            free(temp_str);
            break;
        case NULL_OBJ:
            strcpy(obj_str, "null");
            break;
        default:
            strcpy(obj_str, "Error creating object string.");
            break;
    }

    return obj_str;
}


char* object_list_string(object_list* cur_list) 
{
    char* cur_lit_str; 
    char* list_string = (char*)calloc(256, 1);

    strcpy(list_string, "[");
    for (int i = 0; i < cur_list->count; i++) {
        cur_lit_str = object_string(cur_list->objs[i]);
        if (i == cur_list->count - 1) {
            sprintf(list_string, "%s%s]", list_string, cur_lit_str);
        }
        else {
            sprintf(list_string, "%s%s, ", list_string, cur_lit_str);
        }
        free(cur_lit_str);
    }

    return list_string;
}


char* object_map_string(object_map map) 
{
    char* key_str; 
    char* val_str; 
    char* map_string = (char*)calloc(256, 1);

    int num_found = 0;
    strcpy(map_string, "{");
    for (int i = 0; i < OBJ_MAP_CAPACITY; i++) {
        if (map.entries[i].key.type == NULL_OBJ) {
            continue;
        }
        key_str = object_string(map.entries[i].key);
        val_str = object_string(map.entries[i].val);
        if (num_found == 0) {
            sprintf(map_string, "%s%s: %s", map_string, key_str, val_str);
        }
        else {
            sprintf(map_string, "%s, %s: %s", map_string, key_str, val_str);
        }
        free(key_str);
        free(val_str);

        num_found++;
    }
    sprintf(map_string, "%s}", map_string);

    return map_string;
}


// PRIVATE FUNCTIONS

void _cleanup_token(token tok) {
    free(tok.value);
}

token _copy_token(token tok) {
    token new_tok;
    new_tok.type = tok.type;
    new_tok.value = (char*)malloc(strlen(tok.value)+1);
    strcpy(new_tok.value, tok.value);
    return new_tok;
}

void _cleanup_token_list(token_list tok_lst) {
    for (int i = 0; i < tok_lst.count; i++) {
        _cleanup_token(tok_lst.tokens[i]);
    }
    free(tok_lst.tokens);
}

token_list _copy_token_list(token_list tok_lst) {
    token_list new_tok_lst = new_token_list();
    for (int i = 0; i < tok_lst.count; i++) {
        append_token_list(&new_tok_lst, _copy_token(tok_lst.tokens[i]));
    }
    return new_tok_lst;
}

void _cleanup_expr(expr* ex) {
    switch (ex->type) {
        case INFIX_EXPR:
            _cleanup_expr(ex->data.inf.left);
            _cleanup_expr(ex->data.inf.right);
            break;
        case PREFIX_EXPR:
            _cleanup_expr(ex->data.pre.right);
            break;
        case LITERAL_EXPR:
            _cleanup_literal(ex->data.lit);
            break;
        case IF_EXPR:
            _cleanup_expr(ex->data.ifelse.condition);
            cleanup_stmt_list(ex->data.ifelse.consequence);
            if (ex->data.ifelse.has_alt) {
                cleanup_stmt_list(ex->data.ifelse.alternative);
            }
            break;
        case CALL_EXPR:
            _cleanup_expr_list(ex->data.call.args);
            _cleanup_expr(ex->data.call.func);
            break;
        default:
            assert(false);
    }
    free(ex);
}

expr* _copy_expr(expr* ex) {
    expr* new_ex = (expr*)malloc(sizeof(expr));
    new_ex->type = ex->type;
    switch (ex->type) {
        case INFIX_EXPR:
            new_ex->data.inf.left = _copy_expr(ex->data.inf.left);
            new_ex->data.inf.right = _copy_expr(ex->data.inf.right);
            new_ex->data.inf.op = _copy_token(ex->data.inf.op);
            break;
        case PREFIX_EXPR:
            new_ex->data.pre.right = _copy_expr(ex->data.pre.right);
            new_ex->data.pre.op = _copy_token(ex->data.pre.op);
            break;
        case LITERAL_EXPR:
            new_ex->data.lit = _copy_literal(ex->data.lit);
            break;
        case IF_EXPR:
            new_ex->data.ifelse.condition = _copy_expr(ex->data.ifelse.condition);
            new_ex->data.ifelse.consequence = _copy_stmt_list(ex->data.ifelse.consequence);
            new_ex->data.ifelse.has_alt = ex->data.ifelse.has_alt;
            if (ex->data.ifelse.has_alt) {
                new_ex->data.ifelse.alternative = _copy_stmt_list(ex->data.ifelse.alternative);
            }
            break;
        case CALL_EXPR:
            new_ex->data.call.args = _copy_expr_list(ex->data.call.args);
            new_ex->data.call.func = _copy_expr(ex->data.call.func);
            break;
        default:
            assert(false);
    }
    return new_ex;
}

void _cleanup_expr_list(expr_list ex_lst) {
    for (int i = 0; i < ex_lst.count; i++) {
        _cleanup_expr(ex_lst.exprs[i]);
    }
    free(ex_lst.exprs);
}

expr_list _copy_expr_list(expr_list ex_lst) {
    expr_list new_ex_lst = new_expr_list();
    for (int i = 0; i < ex_lst.count; i++) {
        append_expr_list(&new_ex_lst, _copy_expr(ex_lst.exprs[i]));
    }
    return new_ex_lst;
}

void _cleanup_stmt(stmt st) {
    switch (st.type) {
        case LET_STMT:
            _cleanup_token(st.data.let.identifier);
            _cleanup_expr(st.data.let.value);
            break;
        case EXPR_STMT:
            _cleanup_expr(st.data.expr.value);
            break;
        case RETURN_STMT:
            _cleanup_expr(st.data.ret.value);
            break;
        default:
            break;
    }
}

stmt _copy_stmt(stmt st) {
    stmt new_st;
    new_st.type = st.type;
    switch (st.type) {
        case LET_STMT:
            new_st.data.let.identifier = _copy_token(st.data.let.identifier);
            new_st.data.let.value = _copy_expr(st.data.let.value);
            break;
        case EXPR_STMT:
            new_st.data.expr.value = _copy_expr(st.data.expr.value);
            break;
        case RETURN_STMT:
            new_st.data.ret.value = _copy_expr(st.data.ret.value);
            break;
        default:
            break;
    }
    return new_st;
}

void cleanup_stmt_list(stmt_list st_lst) {
    for (int i = 0; i < st_lst.count; i++) {
        _cleanup_stmt(st_lst.statements[i]);
    }
    free(st_lst.statements);
}

stmt_list _copy_stmt_list(stmt_list st_lst) {
    stmt_list new_st_lst = new_stmt_list();
    for (int i = 0; i < st_lst.count; i++) {
        append_stmt_list(&new_st_lst, _copy_stmt(st_lst.statements[i]));
    }
    return new_st_lst;
}

void _cleanup_literal(literal lit) {
    switch (lit.type) {
        case FN_LIT:
            _cleanup_token_list(lit.data.fn.params);
            cleanup_stmt_list(lit.data.fn.body);
            break;
        case IDENT_LIT:
            _cleanup_token(lit.data.t);
            break;
        default:
            break;
    }
}

literal _copy_literal(literal lit) {
    literal new_lit;
    new_lit.type = lit.type;
    switch (lit.type) {
        case FN_LIT:
            new_lit.data.fn.body = lit.data.fn.body;
            new_lit.data.fn.params = lit.data.fn.params;
            new_lit.data.fn.env = _copy_environment(lit.data.fn.env);
            break;
        case IDENT_LIT:
            new_lit.data.t = lit.data.t;
            break;
        case INT_LIT:
            new_lit.data.i = lit.data.i;
            break;
        case BOOL_LIT:
            new_lit.data.b = lit.data.b;
            break;
        case STRING_LIT:
            new_lit.data.s = (char*)calloc(strlen(lit.data.s)+1, 1);
            strcpy(new_lit.data.s, lit.data.s);
            break;
        default:
            break;
    }
    return new_lit;
}

void cleanup_object(object obj) {
    if (obj.type == ERR_OBJ) {
        free(obj.err);
    }
    if (obj.lit.type == FN_LIT) {
        cleanup_environment(obj.lit.data.fn.env);
    }
}

object copy_object(object obj) {
    object new_obj;
    new_obj.type = obj.type;
    if (obj.type == ERR_OBJ) {
        new_obj.err = (char*)malloc(strlen(obj.err) + 1);
        strcpy(new_obj.err, obj.err);
    }
    else if (obj.type == BUILTIN_OBJ) {
        new_obj.builtin_fn = obj.builtin_fn;
    }
    // TODO: need to figure out how to actually do copies
    else if (obj.type == ARRAY_OBJ) {
        new_obj.arr = obj.arr;
    }
    else {
        new_obj.lit = _copy_literal(obj.lit);
    }

    return new_obj;
}

void _cleanup_env_map(env_map env) {
    env.ref_count -= 1;
    if (env.ref_count > 0) {
        return;
    }
    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        if (env.entries[i].key == NULL) {
            continue;
        }
        free(env.entries[i].key);
        cleanup_object(env.entries[i].value);
    }
}

env_map _copy_env_map(env_map env) {
    env_map new_env = new_env_map();
    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        if (env.entries[i].key == NULL) {
            continue;
        }
        _insert_env(new_env, env.entries[i].key, copy_object(env.entries[i].value));
    }
    return new_env;
}

environment* _copy_environment(environment* env) {
    if (env == NULL) {
        return NULL;
    }
    environment* new_env = (environment*)calloc(1, sizeof(environment));
    new_env->outer = env->outer;
    _increment_env_refs(env->outer);
    return new_env;
}

void cleanup_environment(environment* env) {
    // TODO: fix garbage collector issues
    return;
    env->inner.ref_count -= 1;
    if (env->inner.ref_count <= 0) {
        if (env->inner.entries == NULL) {
            return;
        }
        for (int i = 0; i < ENV_MAP_SIZE; i++) {
            if (env->inner.entries[i].key == NULL) {
                continue;
            }
            free(env->inner.entries[i].key);
            env->inner.entries[i].key = NULL;
            cleanup_object(env->inner.entries[i].value);
        }
        free(env->inner.entries);
        env->inner.entries = NULL;
        return;
    }

    if (env->outer != NULL) {
        cleanup_environment(env->outer);
    }
}

void force_cleanup_environment(environment* env) {
    if (env->inner.entries == NULL) {
        return;
    }

    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        if (env->inner.entries[i].key == NULL) {
            continue;
        }
        free(env->inner.entries[i].key);
        cleanup_object(env->inner.entries[i].value);
    }
    free(env->inner.entries);
    env->inner.entries = NULL;
}

void _increment_env_refs(environment* env) {
    if (env->outer != NULL) {
        _increment_env_refs(env->outer);
    }
    env->inner.ref_count += 1;
}

object create_null_obj()
{
    object null;
    null.type = LIT_OBJ;
    null.lit.type = NULL_LIT;
    null.is_return = false;
    return null;
}

object create_lit_obj(literal l)
{
    object obj;
    obj.type = LIT_OBJ;
    obj.lit = _copy_literal(l);
    obj.is_return = false;
    return obj;
}

object create_builtin_obj(builtin_name name)
{
    object obj;
    obj.type = BUILTIN_OBJ;
    obj.builtin_fn = name;
    obj.is_return = false;
    return obj;
}

object create_err_obj(const char* format, ...)
{
    object obj;
    obj.is_return = false;
    obj.type = ERR_OBJ;

    char* buffer = (char*)malloc(256);
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    obj.err = buffer;
    va_end(args);
    return obj;
}

bool _is_error(object obj)
{
    return obj.type == ERR_OBJ;
}

bool _is_function(object obj)
{
    return obj.lit.type == FN_LIT;
}

int _mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

// Found a random string hash function
unsigned long _get_hash_int(int i, int size)
{
    return _mod(i, size);
}

unsigned long _get_hash_str(char* str, int size)
{
    unsigned long hash = 5381;
    int c;
    while ((c =* str++))
        hash = ((hash << 5) + hash) + c; /* hash*  33 + c */
    return hash % size;
}

unsigned long _get_hash_env(char* str)
{
    return _get_hash_str(str, ENV_MAP_SIZE);
}

void _insert_env(env_map env, char* key, object val)
{
    char* entry_key = (char*)malloc(strlen(key) + 1);
    strcpy(entry_key, key);
    env_map_entry entry = {entry_key, val};
    env.entries[_get_hash_env(entry_key)] = entry;
}

object _get_env(env_map env, char* key)
{
    env_map_entry entry = env.entries[_get_hash_env(key)];
    if (entry.key == NULL)
    {
        return create_err_obj("tried to lookup identifier %s which does not exist", key);
    }
    else if (strcmp(entry.key, key) == 0)
    {
        return env.entries[_get_hash_env(key)].value;
    }
    else
    {
        return create_err_obj("tried to lookup identifier %s but found collision", key);
    }
}

object _lookup_builtins(char* key) 
{
    if (strcmp(key, "len") == 0) {
        return create_builtin_obj(BUILTIN_LEN);
    }
    else if (strcmp(key, "first") == 0) {
        return create_builtin_obj(BUILTIN_FIRST);
    }
    else if (strcmp(key, "last") == 0) {
        return create_builtin_obj(BUILTIN_LAST);
    }
    else if (strcmp(key, "rest") == 0) {
        return create_builtin_obj(BUILTIN_REST);
    }
    else if (strcmp(key, "push") == 0) {
        return create_builtin_obj(BUILTIN_PUSH);
    }
    else if (strcmp(key, "print") == 0) {
        return create_builtin_obj(BUILTIN_PRINT);
    }
    else {
        return create_err_obj("Could not find builtin '%s'.", key);
    }
}

object _get_literal_env(environment* env, char* key) 
{
    object val = _get_env(env->inner, key);
    if (val.type == ERR_OBJ && env->outer != NULL)
    {
        cleanup_object(val);
        return _get_literal_env(env->outer, key);
    }
    return val;
}

object _eval_identifier(environment* env, char* key)
{
    object env_obj = _get_literal_env(env, key);
    if (env_obj.type == ERR_OBJ) {
        cleanup_object(env_obj);
        env_obj = _lookup_builtins(key);
        if (env_obj.type == ERR_OBJ) {
            cleanup_object(env_obj);
            return create_err_obj("Could not resolve identifier %s", key);
        }
    }
    return env_obj;
}

object _cast_as_bool(object l)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object b = create_lit_obj(bool_lit);

    switch (l.lit.type)
    {
    case BOOL_LIT:
        b.lit.data.b = l.lit.data.b;
        break;
    case INT_LIT:
        b.lit.data.b = l.lit.data.i;
        break;
    case FN_LIT:
        b.lit.data.b = l.lit.data.fn.body.count;
        break;
    case STRING_LIT:
        if (strlen(l.lit.data.s) > 0) {
            b.lit.data.b = true;
        }
        else {
            b.lit.data.b = false;
        }
        break;
    case NULL_LIT:
        b.lit.data.b = false;
        break;
    default:
        return create_err_obj("cannot cast object as bool");
    }
    return b;
}

object _eval_prefix(token op, object right)
{
    switch (op.type)
    {
    case BANG:
        right = _cast_as_bool(right);
        right.lit.data.b = !right.lit.data.b;
        return right;
    case MINUS:
        if (right.lit.type == INT_LIT)
        {
            right.lit.data.i = -right.lit.data.i;
            return right;
        }
        return create_err_obj("operator not supported: -");
    default:
        assert(false);
    }
}

object _lit_gt(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type)
    {
        return out;
    }

    switch (left.lit.type)
    {
    case INT_LIT:
        out.lit.type = BOOL_LIT;
        out.lit.data.b = left.lit.data.i > right.lit.data.i;
        return out;
    default:
        return create_err_obj("operator not supported: >");
    }
}

object _lit_lt(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type)
    {
        return out;
    }

    switch (left.lit.type)
    {
    case INT_LIT:
        out.lit.type = BOOL_LIT;
        out.lit.data.b = left.lit.data.i < right.lit.data.i;
        return out;
    default:
        return create_err_obj("operator not supported: <");
    }
}

object _lit_eq(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type)
    {
        return out;
    }

    switch (left.lit.type)
    {
    case INT_LIT:
        out.lit.type = BOOL_LIT;
        out.lit.data.b = left.lit.data.i == right.lit.data.i;
        return out;
    default:
        return create_err_obj("operator not supported: ==");
    }
}

object _lit_neq(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = create_lit_obj(bool_lit);

    if (left.lit.type != right.lit.type)
    {
        return out;
    }

    switch (left.lit.type)
    {
    case INT_LIT:
        out.lit.type = BOOL_LIT;
        out.lit.data.b = left.lit.data.i != right.lit.data.i;
        return out;
    default:
        return create_err_obj("operator not supported: !=");
    }
}

object _index_array(object left, object right) 
{
    if (left.type != ARRAY_OBJ) {
        return create_err_obj("Tried to index object that isnt an array.");
    }
    if (right.type != LIT_OBJ || right.lit.type != INT_LIT) {
        return create_err_obj("Tried to index array with object other than an INT.");
    }
    if (right.lit.data.i >= left.arr.count) {
        return create_err_obj("Array index is out of range.");
    }

    return left.arr.objs[right.lit.data.i];
}

object _index_map(object left, object right) 
{
    if (left.type != MAP_OBJ) {
        return create_err_obj("Tried to index object that isnt a map.");
    }

    return lookup_obj_map(left.map, right);
}

object _eval_infix(token op, object left, object right)
{
    object out = create_null_obj();
    out.type = LIT_OBJ;

    if (left.lit.type == INT_LIT && left.lit.type != right.lit.type)
    {
        return create_err_obj("type mismatch: %s != %s", lit_type_string(left.lit), lit_type_string(right.lit));
    }

    switch (op.type)
    {
    case PLUS:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i + right.lit.data.i;
            return out;
        }
        else if (left.lit.type == STRING_LIT)
        {
            out.lit.type = STRING_LIT;
            out.lit.data.s = (char*)calloc(strlen(left.lit.data.s) + strlen(right.lit.data.s) + 1, 1);
            sprintf(out.lit.data.s, "%s%s", left.lit.data.s, right.lit.data.s);
            return out;
        }
        return create_err_obj("operator not supported: %s", op.value);
    case MINUS:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i - right.lit.data.i;
            return out;
        }
        return create_err_obj("operator not supported: %s", op.value);
    case ASTERISK:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i*  right.lit.data.i;
            return out;
        }
        return create_err_obj("operator not supported: %s", op.value);
    case SLASH:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i / right.lit.data.i;
            return out;
        }
        return create_err_obj("operator not supported: %s", op.value);
    case GT:
        return _lit_gt(left, right);
    case LT:
        return _lit_lt(left, right);
    case EQ:
        return _lit_eq(left, right);
    case NOT_EQ:
        return _lit_neq(left, right);
    case LBRACKET:
        if (left.type == ARRAY_OBJ) {
            return _index_array(left, right);
        }
        else if (left.type == MAP_OBJ) {
            return _index_map(left, right);
        }
        else {
            assert(false);
        }
    default:
        return create_err_obj("operator not supported: %s", op.value);
    }
}

object _eval_array(environment* env, expr_list ex_arr)
{
    object out_arr = create_null_obj();
    out_arr.type = ARRAY_OBJ;
    out_arr.arr = new_object_list();

    object cur_obj = create_null_obj();
    for (int i = 0; i < ex_arr.count; i++) {
        cur_obj = _eval_expr(ex_arr.exprs[i], env);
        if (_is_error(cur_obj)) {
            return cur_obj;
        }
        append_object_list(&out_arr.arr, cur_obj);
    }
    return out_arr;
}

object _eval_map(environment* env, expr_pair_list ex_pairs)
{
    object out_map = create_null_obj();
    out_map.type = MAP_OBJ;
    out_map.map = new_object_map();

    object key_obj = create_null_obj();
    object val_obj = create_null_obj();
    for (int i = 0; i < ex_pairs.count; i++) {
        key_obj = _eval_expr(ex_pairs.expr_pairs[i].first, env);
        if (_is_error(key_obj)) {
            return key_obj;
        }
        val_obj = _eval_expr(ex_pairs.expr_pairs[i].second, env);
        if (_is_error(val_obj)) {
            return val_obj;
        }
        insert_obj_map(out_map.map, key_obj, val_obj);
    }
    return out_map;
}

object _eval_expr(expr* e, environment* env)
{
    object out = create_null_obj();
    object left, right;
    object func;
    expr_list args;

    switch (e->type)
    {
    case LITERAL_EXPR:
        // Resolve any identifier
        if (e->data.lit.type == IDENT_LIT)
        {
            out = _eval_identifier(env, e->data.lit.data.t.value);
            if (_is_error(out)) {
                return out;
            }
            else {
                // Need to make a copy cause of how garbage collection works
                return copy_object(out);
            }
        }
        else if (e->data.lit.type == ARRAY_LIT) {
            return _eval_array(env, e->data.lit.data.arr);
        }
        else if (e->data.lit.type == MAP_LIT) {
            return _eval_map(env, e->data.lit.data.map);
        }
        else if (e->data.lit.type == FN_LIT) {
            out = create_lit_obj(e->data.lit);
            out.lit.data.fn.env = (environment*)calloc(1, sizeof(environment));
            out.lit.data.fn.env->outer = env;
            _increment_env_refs(out.lit.data.fn.env->outer);
            return out;
        }
        else {
            return create_lit_obj(e->data.lit);
        }
    case PREFIX_EXPR:
        out = _eval_expr(e->data.pre.right, env);
        if (_is_error(out))
        {
            return out;
        }
        return _eval_prefix(e->data.pre.op, out);
    case INFIX_EXPR:
        left = _eval_expr(e->data.inf.left, env);
        if (_is_error(left))
        {
            return left;
        }
        right = _eval_expr(e->data.inf.right, env);
        if (_is_error(right))
        {
            return right;
        }
        return _eval_infix(e->data.inf.op, left, right);
    case IF_EXPR:
        out = _eval_expr(e->data.ifelse.condition, env);
        if (_is_error(out))
        {
            return out;
        }
        out = _cast_as_bool(out);
        if (_is_error(out))
        {
            return out;
        }
        if (out.lit.data.b)
        {
            return eval_program(&e->data.ifelse.consequence, env);
        }
        else if (e->data.ifelse.has_alt)
        {
            return eval_program(&e->data.ifelse.alternative, env);
        }
        else
        {
            return create_null_obj();
        }
    case CALL_EXPR:
        // Copy of function with incremented environment
        func = _eval_expr(e->data.call.func, env);
        if (_is_error(func))
        {
            return func;
        }

        args = e->data.call.args;

        if (func.type == LIT_OBJ) {
            if (args.count != func.lit.data.fn.params.count)
            {
                return create_err_obj("arg mismatch, input: %d fn: %d", args.count, func.lit.data.fn.params.count);
            }

            func.lit.data.fn.env->inner = new_env_map();
            for (int i = 0; i < args.count; i++)
            {
                out = _eval_expr(args.exprs[i], env);
                if (_is_error(out))
                {
                    return out;
                }
                _insert_env(func.lit.data.fn.env->inner, func.lit.data.fn.params.tokens[i].value, out);
            }
            out = eval_program(&func.lit.data.fn.body, func.lit.data.fn.env);
            cleanup_object(func);
            return out;
        }
        else if (func.type == BUILTIN_OBJ) {
            object_list resolved_args = new_object_list();
            for (int i = 0; i < args.count; i++)
            {
                out = _eval_expr(args.exprs[i], env);
                if (_is_error(out))
                {
                    return out;
                }
                append_object_list(&resolved_args, out);
            }
            out = call_builtin(func, resolved_args);
            cleanup_object(func);
            return out;
        }
    default:
        return create_null_obj();
    }
}

object _eval_stmt(stmt* s, environment* env)
{
    object out = create_null_obj();

    switch (s->type)
    {
    // Can I just combine ret into expr
    case RETURN_STMT:
        out = _eval_expr(s->data.ret.value, env);
        if (_is_error(out))
        {
            return out;
        }
        out.is_return = true;
        return out;
    case EXPR_STMT:
        out = _eval_expr(s->data.expr.value, env);
        return out;
    case LET_STMT:
        out = _eval_expr(s->data.let.value, env);
        if (_is_error(out))
        {
            return out;
        }
        _insert_env(env->inner, s->data.let.identifier.value, out);
        return create_null_obj();
    default:
        return create_null_obj();
    }
}