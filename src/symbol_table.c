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

Symbol *find_symbol_scope(Scope *scope, char *name) {
    for (int i = 0; i < scope->num_symbols; i++) {
        if (strcmp(scope->symbols[i]->name, name) == 0) {
            return scope->symbols[i];
        }
    }
    if (scope->parent != NULL) {
        return find_symbol_scope(scope->parent, name);
    }
    else {
        return NULL;
    }
}

Symbol *find_symbol(Symbol_Table *table, char *name) {
    Scope *scope = table->current_scope;
    for (int i = 0; i < scope->num_symbols; i++) {
        if (strcmp(scope->symbols[i]->name, name) == 0) {
            return scope->symbols[i];
        }
    }
    if (scope->parent != NULL) {
        return find_symbol_scope(scope->parent, name);
    }
    else {
        return NULL;
    }
}

Symbol *create_symbol(char *name, enum Symbol_Type type, void *value, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node) {
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    new_symbol->name = name;
    new_symbol->type = type;
    switch (type) {
    case SYM_INT:
        new_symbol->value.int_val = *((int *)value);
        break;
    case SYM_BOOL:
        new_symbol->value.bool_val = *((int *)value);
        break;
    case SYM_STR:
        new_symbol->value.string_val = (char *)value;
        break;
    case SYM_FUNC:
        new_symbol->value.func_val.param_names = param_names;
        new_symbol->value.func_val.param_types = param_types;
        new_symbol->value.func_val.num_params = num_params;
        new_symbol->value.func_val.return_type = return_type;
        new_symbol->value.func_val.func_node = func_node;
        break;
    }
    return new_symbol;
}

Symbol *create_symbol_int(char *name, int value) {
    int *val = malloc(sizeof(int));
    *val = value;
    return create_symbol(name, SYM_INT, val, NULL, NULL, 0, 0, NULL);
}

Symbol *create_symbol_string(char *name, char *value) {
    return create_symbol(name, SYM_STR, value, NULL, NULL, 0, 0, NULL);
}

Symbol *create_symbol_bool(char *name, int value) {
    int *val = malloc(sizeof(int));
    *val = value;
    return create_symbol(name, SYM_BOOL, val, NULL, NULL, 0, 0, NULL);
}

void set_existing_symbol(Symbol_Table *table, Symbol *symbol) {
    Scope *current_scope = table->current_scope;
    while (current_scope != NULL) {
        for (int i = 0; i < current_scope->num_symbols; i++) {
            if (strcmp(current_scope->symbols[i]->name, symbol->name) == 0) {
                current_scope->symbols[i] = symbol;
                return;
            }
        }
        current_scope = current_scope->parent;
    }
    current_scope = table->current_scope;
    current_scope->num_symbols++;
    current_scope->symbols = (Symbol**)realloc(current_scope->symbols, sizeof(Symbol*) * current_scope->num_symbols);
    current_scope->symbols[current_scope->num_symbols - 1] = symbol;
}

void set_symbol(Symbol_Table *table, char *name, enum Symbol_Type type, void *value, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node) {
    Symbol *symbol = find_symbol_scope(table->current_scope, name);
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
