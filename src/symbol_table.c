#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

Symbol_Table *create_symbol_table() {
    Symbol_Table *table = malloc(sizeof(Symbol_Table));
    table->current_scope = create_scope(NULL);
    return table;
}

Scope *create_scope(Scope *parent) {
    Scope *scope = malloc(sizeof(Scope));
    scope->num_symbols = 0;
    scope->parent = parent;
    scope->symbols = NULL;
    return scope;
}

void push_scope(Symbol_Table *table) {
    Scope *new_scope = create_scope(table->current_scope);
    table->current_scope = new_scope;
}

void pop_scope(Symbol_Table *table) {
    Scope *old_scope = table->current_scope;
    table->current_scope = table->current_scope->parent;
    free(old_scope);
}

Symbol *find_symbol(Scope *scope, char *name) {
    for (int i = 0; i < scope->num_symbols; i++) {
        if (strcmp(scope->symbols[i]->name, name) == 0) {
            return scope->symbols[i];
        }
    }
    if (scope->parent != NULL) {
        return find_symbol(scope->parent, name);
    }
    else {
        return NULL;
    }
}

void set_symbol(Symbol_Table *table, char *name, enum Symbol_Type type, void *value, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node) {
    Symbol *symbol = find_symbol(table->current_scope, name);
    if (symbol == NULL) {
        symbol = (Symbol *)malloc(sizeof(Symbol));
        symbol->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
        strcpy(symbol->name, name);
        table->current_scope->num_symbols++;
        table->current_scope->symbols = (Symbol **)realloc(table->current_scope->symbols, sizeof(Symbol *) * table->current_scope->num_symbols);
        table->current_scope->symbols[table->current_scope->num_symbols - 1] = symbol;
    }
    symbol->type = type;
    switch (type) {
    case SYM_INT:
        symbol->value.int_val = *(int *)value;
        break;
    case SYM_BOOL:
        symbol->value.bool_val = *(int *)value;
        break;
    case SYM_STR:
        symbol->value.string_val = (char *)malloc(sizeof(char) * (strlen((char *)value) + 1));
        strcpy(symbol->value.string_val, (char *)value);
        break;
    case SYM_FUNC:
        symbol->value.func_val.param_names = param_names;
        symbol->value.func_val.param_types = param_types;
        symbol->value.func_val.num_params = num_params;
        symbol->value.func_val.return_type = return_type;
        symbol->value.func_val.func_node = func_node;
        break;
    }
}

void set_symbol_int(Symbol_Table *table, char *name, int value) {
    set_symbol(table, name, SYM_INT, &value, NULL, NULL, 0, SYM_INT, NULL);
}

void set_symbol_bool(Symbol_Table *table, char *name, int value) {
    set_symbol(table, name, SYM_BOOL, &value, NULL, NULL, 0, SYM_BOOL, NULL);
}

void set_symbol_string(Symbol_Table *table, char *name, char *value) {
    set_symbol(table, name, SYM_STR, value, NULL, NULL, 0, SYM_STR, NULL);
}

void set_symbol_func(Symbol_Table *table, char *name, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node) {
    set_symbol(table, name, SYM_FUNC, NULL, param_names, param_types, num_params, return_type, func_node);
}
