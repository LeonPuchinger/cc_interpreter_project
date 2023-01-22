#ifndef _DEBUG_H
#define _DEBUG_H

#include "types.h"
#include <stdio.h>

void debug_traverse_tree(AST_Node *node, int level) {
    // Output a tree view of the AST for debug purposes
    for (int i = 0; i < level; i++) {
        printf("  ");
    }
    switch (node->type) {
    case ND_FUNC_DEFS:
        printf("ND_FUNC_DEFS\n");
        break;
    case ND_FUNC_DEF:
        printf("ND_FUNC_DEF\n");
        break;
    case ND_STMTS:
        printf("ND_STMTS\n");
        break;
    case ND_INT:
        printf("ND_INT: %d\n", node->int_value);
        break;
    case ND_STR:
        printf("ND_STR: %s\n", node->str_value);
        break;
    case ND_ASSIGN:
        printf("ND_ASSIGN\n");
        break;
    case ND_PARAMS:
        printf("ND_PARAMS\n");
        break;
    case ND_TYPE:
        printf("ND_TYPE: %d\n", node->subtype);
        break;
    case ND_COND:
        printf("ND_COND\n");
        break;
    case ND_LOOP:
        printf("ND_LOOP\n");
        break;
    case ND_BOOL_EXPR:
        printf("ND_BOOL_EXPR\n");
        break;
    case ND_STR_EXPR:
        printf("ND_STR_EXPR: %d\n", node->subtype);
        break;
    case ND_INT_EXPR:
        printf("ND_INT_EXPR: %d\n", node->subtype);
        break;
    case ND_FUNC_CALL:
        printf("ND_FUNC_CALL: %d\n", node->subtype);
        break;
    case ND_EXPRS:
        printf("ND_EXPRS\n");
        break;
    default:
        printf("Unknown node type\n");
        break;
    }
    for (int i = 0; i < node->children_size; i += 1) {
        debug_traverse_tree(node->children[i], level + 1);
    }
}

void debug_print_symbol_table(Symbol_Table *table, int level) {
    for (int i = 0; i < table->current_scope->num_symbols; i++) {
        Symbol *symbol = table->current_scope->symbols[i];
        for (int j = 0; j < level; j++) {
            printf("    ");
        }
        printf("Name: %s, Type: %d, ", symbol->name, symbol->type);
        switch (symbol->type) {
        case SYM_INT:
            printf("Value: %d\n", symbol->value.int_val);
            break;
        case SYM_BOOL:
            printf("Value: %d\n", symbol->value.bool_val);
            break;
        case SYM_STR:
            printf("Value: %s\n", symbol->value.string_val);
            break;
        case SYM_FUNC:
            printf("Return Type: %d, Number of Parameters: %d\n", symbol->value.func_val.return_type, symbol->value.func_val.num_params);
            printf("    Parameters: \n");
            for (int k = 0; k < symbol->value.func_val.num_params; k++) {
                for (int j = 0; j < level + 1; j++) {
                    printf("    ");
                }
                printf("Name: %s, Type: %d\n", symbol->value.func_val.param_names[k], symbol->value.func_val.param_types[k]);
            }
            printf("    AST Node: %p\n", symbol->value.func_val.func_node);
            break;
        }
    }
}

#endif
