#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "expressions.h"


token_list new_token_list() {
    token_list new_list;
    new_list.capacity = 1;
    new_list.tokens = (token*)malloc(new_list.capacity * sizeof(token));
    new_list.count = 0;
    return new_list;
}

void append_token_list(token_list* cur_list, token new_token) {
    if ((cur_list->count) >= cur_list->capacity) {
        cur_list->tokens = (token*)realloc(cur_list->tokens, cur_list->capacity * 2 * sizeof(token));
        cur_list->capacity *= 2;
    }
    cur_list->tokens[cur_list->count] = new_token;
    cur_list->count++;
}

char* token_list_string(token_list* tl) {
    char* tl_str = malloc(128);
    char* tmp_str = malloc(128);
    strcpy(tl_str, "");

    for(int i = 0; i < tl->count; i++) {
        if (i == tl->count - 1) {
            strcpy(tmp_str, tl_str);
            sprintf(tl_str, "%s%s", tmp_str, tl->tokens[i].value);
        } else {
            strcpy(tmp_str, tl_str);
            sprintf(tl_str, "%s%s, ", tmp_str, tl->tokens[i].value);
        }
    }

    free(tmp_str);
    return tl_str;
}



expr_list new_expr_list() {
    expr_list new_list;
    new_list.capacity = 1;
    new_list.exprs = (expr**)malloc(new_list.capacity * sizeof(expr*));
    new_list.count = 0;
    return new_list;
}

void append_expr_list(expr_list* cur_list, expr* new_expr) {
    if ((cur_list->count) >= cur_list->capacity) {
        cur_list->exprs = (expr**)realloc(cur_list->exprs, cur_list->capacity * 2 * sizeof(expr*));
        cur_list->capacity *= 2;
    }
    cur_list->exprs[cur_list->count] = new_expr;
    cur_list->count++;
}

char* expr_list_string(expr_list* el) {
    char* el_str = malloc(128);
    char* tmp_str = malloc(128);
    strcpy(el_str, "");

    for(int i = 0; i < el->count; i++) {
        if (i == el->count - 1) {
            strcpy(tmp_str, el_str);
            sprintf(el_str, "%s%s", tmp_str, expression_string(el->exprs[i]));
        } else {
            strcpy(tmp_str, el_str);
            sprintf(el_str, "%s%s, ", tmp_str, expression_string(el->exprs[i]));
        }
    }

    free(tmp_str);
    return el_str;
}


char* literal_string(literal lit) {
    char* lit_str = malloc(128);

    switch (lit.type) {
        case IDENT_LIT:
            strcpy(lit_str, lit.data.t.value);
            break;
        case INT_LIT:
            sprintf(lit_str, "%d", lit.data.i);
            break;
        case BOOL_LIT:
            sprintf(lit_str, "%s", lit.data.b ? "true" : "false");
            break;
        case FN_LIT:
            sprintf(lit_str, "fn(%s) { %s }", token_list_string(&lit.data.fn.params), program_string(&lit.data.fn.body));
            break;
        default:
            strcpy(lit_str, "");
    }

    return lit_str;
}


char* lit_type_string(literal lit) {
    char* lit_str = malloc(64);

    switch (lit.type) {
        case IDENT_LIT:
            strcpy(lit_str, "Identifier");
            break;
        case INT_LIT:
            strcpy(lit_str, "Int");
            break;
        case BOOL_LIT:
            strcpy(lit_str, "Bool");
            break;
        case FN_LIT:
            strcpy(lit_str, "Function");
            break;
        default:
            strcpy(lit_str, "Null");
    }

    return lit_str;
}


char* prefix_string(struct prefix_expr pre) {
    char* pre_str = malloc(128);

    sprintf(pre_str, "%s%s", pre.op.value, expression_string(pre.right));

    return pre_str;
}


char* infix_string(struct infix_expr inf) {
    char* inf_str = malloc(128);

    sprintf(inf_str, "(%s %s %s)", expression_string(inf.left), inf.op.value, expression_string(inf.right));

    return inf_str;
}


char* if_string(struct if_expr ifelse) {
    char* if_str = malloc(128);

    if (ifelse.has_alt) {
        sprintf(if_str, "if (%s) { %s } else { %s }", expression_string(ifelse.condition), program_string(&ifelse.consequence), program_string(&ifelse.alternative));
    }
    else {
        sprintf(if_str, "if (%s) { %s }", expression_string(ifelse.condition), program_string(&ifelse.consequence));
    }

    return if_str;
}


char* call_string(struct call_expr call) {
    char* call_str = malloc(128);

    sprintf(call_str, "%s(%s)", expression_string(call.func), expr_list_string(&call.args));

    return call_str;
}


char* expression_string(expr* e) {
    char* expr_str;

    switch (e->type) {
        case LITERAL_EXPR:
            return literal_string(e->data.lit);
        case PREFIX_EXPR:
            return prefix_string(e->data.pre);
        case INFIX_EXPR:
            return infix_string(e->data.inf);
        case IF_EXPR:
            return if_string(e->data.ifelse);
        case CALL_EXPR:
            return call_string(e->data.call);
        default:
            expr_str = malloc(128);
            strcpy(expr_str, "");
            return expr_str;
    }
}