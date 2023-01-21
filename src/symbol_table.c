#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

Scope *create_scope(Scope *parent) {
    Scope *scope = (Scope *)malloc(sizeof(Scope));
    scope->num_symbols = 0;
    scope->parent = parent;
    scope->symbols = NULL;
    return scope;
}

void push_scope(SymbolTable *table) {
    Scope *new_scope = create_scope(table->current_scope);
    table->current_scope = new_scope;
}

void pop_scope(SymbolTable *table) {
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

void set_symbol(SymbolTable *table, char *name, enum Symbol_Type type, void *value) {
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
    }
}
