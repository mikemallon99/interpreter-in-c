#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "eval.h"

#define ENV_MAP_SIZE 128

object _create_null_obj();
object _create_lit_obj(literal l);
object _create_err_obj(const char* format, ...);

bool _is_function(object obj);
bool _is_error(object obj);
object _cast_as_bool(object l);

int _get_hash_env(char* str);
void _insert_env(env_map map, char* key, object val);
object _get_env(env_map map, char* key);
object _get_literal(environment* env, char* key);

object _lit_gt(object left, object right);
object _lit_lt(object left, object right);
object _lit_eq(object left, object right);
object _lit_neq(object left, object right);
object _eval_prefix(token op, object right);
object _eval_infix(token op, object left, object right);
object _eval_expr(expr* e, environment* env);
object _eval_stmt(stmt* s, environment* env);

void _cleanup_token(token tok);
void _cleanup_token_list(token_list tok_lst);
void _cleanup_expr(expr* ex);
void _cleanup_expr_list(expr_list ex_lst);
void _cleanup_stmt(stmt st);
void _cleanup_stmt_list(stmt_list st_lst);
void _cleanup_literal(literal lit);
void _cleanup_object(object obj);
void _cleanup_env_map(env_map env);
void _cleanup_environment(environment* env);

token _copy_token(token tok);
token_list _copy_token_list(token_list tok_lst);
expr* _copy_expr(expr* ex);
expr_list _copy_expr_list(expr_list ex_lst);
stmt _copy_stmt(stmt st);
stmt_list _copy_stmt_list(stmt_list st_lst);
literal _copy_literal(literal lit);
object _copy_object(object obj);
env_map _copy_env_map(env_map env);
environment* _copy_environment(environment* env);

void _increment_env_refs(environment* env);

// PUBLIC FUNCTIONS

env_map new_env_map()
{
    env_map env;
    env.entries = (env_map_entry* )calloc(ENV_MAP_SIZE, sizeof(env_map_entry));
    env.ref_count = 1;
    return env;
}

object eval_program(stmt_list* p, environment* env)
{
    object out;

    for (int i = 0; i < p->count; i++)
    {
        out = _eval_stmt(&p->statements[i], env);
        if (out.type == RET_OBJ || out.type == ERR_OBJ)
        {
            break;
        }
        if (i != p->count - 1) {
            _cleanup_object(out);
        }
    }


    return out;
}


// PRIVATE FUNCTIONS

object _object_copy(object obj) {
    object new_obj;
    strcpy(new_obj.err, obj.err);
    new_obj.type = obj.type;
    new_obj.lit.type = obj.lit.type;
    new_obj.lit.data = obj.lit.data;
}

void _cleanup_token(token tok) {
    free(tok.value);
}

token _copy_token(token tok) {
    token new_tok;
    new_tok.type = tok.type;
    new_tok.value = malloc(strlen(tok.value)+1);
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
            _cleanup_token(ex->data.inf.op);
            break;
        case PREFIX_EXPR:
            _cleanup_expr(ex->data.pre.right);
            _cleanup_token(ex->data.pre.op);
            break;
        case LITERAL_EXPR:
            _cleanup_literal(ex->data.lit);
            break;
        case IF_EXPR:
            _cleanup_expr(ex->data.ifelse.condition);
            _cleanup_stmt_list(ex->data.ifelse.consequence);
            if (ex->data.ifelse.has_alt) {
                _cleanup_stmt_list(ex->data.ifelse.alternative);
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
    expr* new_ex = malloc(sizeof(expr));
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

void _cleanup_stmt_list(stmt_list st_lst) {
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
            _cleanup_stmt_list(lit.data.fn.body);
            _cleanup_environment(lit.data.fn.env);
            break;
        case IDENT_LIT:
            _cleanup_token(lit.data.t);
        default:
            break;
    }
}

literal _copy_literal(literal lit) {
    literal new_lit;
    new_lit.type = lit.type;
    switch (lit.type) {
        case FN_LIT:
            new_lit.data.fn.body = _copy_stmt_list(lit.data.fn.body);
            new_lit.data.fn.params = _copy_token_list(lit.data.fn.params);
            new_lit.data.fn.env = _copy_environment(lit.data.fn.env);
            break;
        case IDENT_LIT:
            new_lit.data.t = _copy_token(lit.data.t);
            break;
        case INT_LIT:
            new_lit.data.i = lit.data.i;
            break;
        case BOOL_LIT:
            new_lit.data.b = lit.data.b;
            break;
        default:
    }
    return new_lit;
}

void _cleanup_object(object obj) {
    if (obj.type == ERR_OBJ) {
        free(obj.err);
    }
    _cleanup_literal(obj.lit);
}

object _copy_object(object obj) {
    object new_obj;
    new_obj.type = obj.type;
    if (obj.type == ERR_OBJ) {
        new_obj.err = malloc(strlen(obj.err) + 1);
        strcpy(new_obj.err, obj.err);
    }
    new_obj.lit = _copy_literal(obj.lit);
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
        _cleanup_object(env.entries[i].value);
    }
}

env_map _copy_env_map(env_map env) {
    env_map new_env = new_env_map();
    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        if (env.entries[i].key == NULL) {
            continue;
        }
        _insert_env(new_env, env.entries[i].key, _copy_object(env.entries[i].value));
    }
    return new_env;
}

environment* _copy_environment(environment* env) {
    if (env == NULL) {
        return NULL;
    }
    environment* new_env = calloc(1, sizeof(environment));
    new_env->outer = env->outer;
    new_env->inner = env->inner;
    _increment_env_refs(env);
    return new_env;
}

void _cleanup_environment(environment* env) {
    if (env->outer != NULL) {
        _cleanup_environment(env->outer);
    }
    env->inner.ref_count -= 1;
    if (env->inner.ref_count > 0) {
        return;
    }
    for (int i = 0; i < ENV_MAP_SIZE; i++) {
        if (env->inner.entries[i].key == NULL) {
            continue;
        }
        free(env->inner.entries[i].key);
        _cleanup_object(env->inner.entries[i].value);
    }
}

void _increment_env_refs(environment* env) {
    if (env->outer != NULL) {
        _increment_env_refs(env->outer);
    }
    env->inner.ref_count += 1;
}

object _create_null_obj()
{
    object null;
    null.type = LIT_OBJ;
    null.lit.type = NULL_LIT;
    return null;
}

object _create_lit_obj(literal l)
{
    object obj;
    obj.type = LIT_OBJ;
    obj.lit = l;
    return obj;
}

object _create_err_obj(const char* format, ...)
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

bool _is_error(object obj)
{
    return obj.type == ERR_OBJ;
}

bool _is_function(object obj)
{
    return obj.lit.type == FN_LIT;
}

// Found a random string hash function
int _get_hash_env(char* str)
{
    int hash = 5381;
    int c;
    while ((c =* str++))
        hash = ((hash << 5) + hash) + c; /* hash*  33 + c */
    return hash % ENV_MAP_SIZE;
}

void _insert_env(env_map env, char* key, object val)
{
    env_map_entry entry = {key, val};
    env.entries[_get_hash_env(key)] = entry;
}

object _get_env(env_map env, char* key)
{
    env_map_entry entry = env.entries[_get_hash_env(key)];
    if (entry.key == NULL)
    {
        return _create_err_obj("tried to lookup identifier %s which does not exist", key);
    }
    else if (strcmp(entry.key, key) == 0)
    {
        return env.entries[_get_hash_env(key)].value;
    }
    else
    {
        return _create_err_obj("tried to lookup identifier %s but found collision", key);
    }
}

object _get_literal(environment* env, char* key)
{
    object val;
    val = _get_env(env->inner, key);
    if (val.type == ERR_OBJ && env->outer != NULL)
    {
        return _get_literal(env->outer, key);
    }
    return val;
}

object _cast_as_bool(object l)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object b = _create_lit_obj(bool_lit);

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
    case NULL_LIT:
        b.lit.data.b = false;
        break;
    default:
        return _create_err_obj("cannot cast object as bool");
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
        return _create_err_obj("operator not supported: -");
    default:
        assert(false);
    }
}

object _lit_gt(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

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
        return _create_err_obj("operator not supported: >");
    }
}

object _lit_lt(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

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
        return _create_err_obj("operator not supported: <");
    }
}

object _lit_eq(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

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
        return _create_err_obj("operator not supported: ==");
    }
}

object _lit_neq(object left, object right)
{
    literal bool_lit;
    bool_lit.type = BOOL_LIT;
    object out = _create_lit_obj(bool_lit);

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
        return _create_err_obj("operator not supported: !=");
    }
}

object _eval_infix(token op, object left, object right)
{
    object out;

    if (left.lit.type != right.lit.type)
    {
        return _create_err_obj("type mismatch: %s != %s", lit_type_string(left.lit), lit_type_string(right.lit));
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
        return _create_err_obj("operator not supported: %s", op.value);
    case MINUS:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i - right.lit.data.i;
            return out;
        }
        return _create_err_obj("operator not supported: %s", op.value);
    case ASTERISK:
        if (left.lit.type == INT_LIT)
        {
            out.lit.type = INT_LIT;
            out.lit.data.i = left.lit.data.i*  right.lit.data.i;
            return out;
        }
        return _create_err_obj("operator not supported: %s", op.value);
    case SLASH:
        if (left.lit.type == INT_LIT)
        {
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

object _eval_expr(expr* e, environment* env)
{
    object out;
    object left, right;
    object func;
    expr_list args;

    switch (e->type)
    {
    case LITERAL_EXPR:
        // Resolve any identifier
        if (e->data.lit.type == IDENT_LIT)
        {
            return _copy_object(_get_literal(env, e->data.lit.data.t.value));
        }
        else if (e->data.lit.type == FN_LIT)
        {
            e->data.lit.data.fn.env = (environment*)calloc(1, sizeof(environment));
            e->data.lit.data.fn.env->outer = env;
            _increment_env_refs(e->data.lit.data.fn.env->outer);
        }
        return _create_lit_obj(e->data.lit);
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
            return _create_null_obj();
        }
    case CALL_EXPR:
        func = _eval_expr(e->data.call.func, env);
        if (_is_error(func))
        {
            return func;
        }

        args = e->data.call.args;
        if (args.count != func.lit.data.fn.params.count)
        {
            _create_err_obj("arg mismatch, input: %d fn: %d", args.count, func.lit.data.fn.params.count);
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
        _cleanup_environment(env);
        return out;
    default:
        return _create_null_obj();
    }
}

object _eval_stmt(stmt* s, environment* env)
{
    object out;

    switch (s->type)
    {
    // Can I just combine ret into expr
    case RETURN_STMT:
        out = _eval_expr(s->data.ret.value, env);
        if (_is_error(out))
        {
            return out;
        }
        out.type = RET_OBJ;
        return out;
    case EXPR_STMT:
        return _eval_expr(s->data.expr.value, env);
    case LET_STMT:
        out = _eval_expr(s->data.let.value, env);
        if (_is_error(out))
        {
            return out;
        }
        _insert_env(env->inner, s->data.let.identifier.value, out);
        return _create_null_obj();
    default:
        return _create_null_obj();
    }
}