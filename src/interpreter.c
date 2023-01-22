#include <stdio.h>
#include <stdlib.h>
#include "../gen/parser.tab.h"
#include "types.h"
#include "debug.h"

AST_Node *root;

enum Symbol_Type subtype_to_symbol_type(int subtype);
void register_funcs(AST_Node *root, Symbol_Table *table);
Symbol *check_entry_point(Symbol_Table *table);
void execute_assign(Symbol_Table *table, Symbol_Table *global_table, AST_Node *assign);
void execute_stmt(Symbol_Table *table, Symbol_Table *global_table, AST_Node *stmt, Symbol *function);
void *execute_return(Symbol_Table *table, AST_Node *ret, Symbol *function);
void prepare_function_call(Symbol_Table *table, Symbol_Table *global_table, Symbol *function, AST_Node *exprs);
Symbol *execute_function(Symbol_Table *table, Symbol_Table *global_table, Symbol *function);

enum Symbol_Type subtype_to_symbol_type(int subtype) {
    // Helper function to translate an AST subtype into a Symbol_Type.
    // The function uses the subtypes of "TYPE_ANNOT" AST node.
    switch (subtype) {
    case 0:
        // str param
        return SYM_STR;
    case 1:
        // int param
        return SYM_INT;
    case 2:
        // bool param
        return SYM_BOOL;
    }
}

void register_funcs(AST_Node *root, Symbol_Table *table) {
    // Collect all functions in the AST and register them in the symbol table.
    // For each function, the name, parameters (with types) and the return type is saved.
    for (int i = 0; i < root->children_size; i += 1) {
        // iterate over functions
        // the root node should only contain functions
        AST_Node *func = root->children[i];
        char *func_name = NULL;
        if (func->children_size >= 1 && func->children[0]->type == ND_STR) {
            // function has name
            func_name = func->children[0]->str_value;
        }
        char **param_names = NULL;
        enum Symbol_Type *param_types = NULL;
        int param_count = 0;
        if (func->children_size >= 2 && func->children[1]->type == ND_PARAMS) {
            // function has params
            AST_Node *params = func->children[1];
            param_count = params->children_size / 2;
            param_names = calloc(param_count, sizeof(char *));
            param_types = calloc(param_count, sizeof(enum Symbol_Type));
            for (int j = 0; j < params->children_size; j += 2) {
                // iterate over params (extract name & type)
                param_names[j / 2] = params->children[j]->str_value;
                param_types[j / 2] = subtype_to_symbol_type(params->children[j + 1]->subtype);
            }
        }
        enum Symbol_Type return_type;
        if (func->children[func->children_size - 2]->type == ND_TYPE) {
            // function has return type
            return_type = subtype_to_symbol_type(func->children[func->children_size - 2]->subtype);
        }
        AST_Node *stmts = NULL;
        if (func->children[func->children_size - 1]->type == ND_STMTS) {
            // function has statements
            stmts = func->children[func->children_size - 1];
        }
        set_symbol_func(table, func_name, param_names, param_types, param_count, return_type, stmts);
    }
}

Symbol *check_entry_point(Symbol_Table *table) {
    Symbol *begin = find_symbol(table, "begin");
    if (begin == NULL) {
        printf("ERROR: entry point not found. please define 'begin' function.\n");
        exit(1);
    }
    if (begin->value.func_val.num_params != 0) {
        printf("ERROR: 'begin' function does not take parameters\n");
        exit(1);
    }
    if (begin->value.func_val.return_type != SYM_INT) {
        printf("ERROR: 'begin' function needs to have 'int' return type\n");
        exit(1);
    }
    return begin;
}

void execute_assign(Symbol_Table *table, Symbol_Table *global_table, AST_Node *assign) {
    char *variable_name = assign->children[0]->str_value;
    if (find_symbol(global_table, variable_name) != NULL) {
        printf("ERROR: cannot reassign functions (%s).\n", variable_name);
        exit(1);
    }
    Symbol *existing = find_symbol(table, variable_name);
    // TODO: evaluate expression
    if (existing == NULL) {
        // new symbol needs to be registered
        
        return;
    }
    // symbol already exists, check type and assign
    // TODO
}

void *execute_return(Symbol_Table *table, AST_Node *ret, Symbol *function) {
    switch (ret->subtype) {
    case 0:
        // assigning ident
        char *ident_name = ret->children[0]->str_value;
        Symbol *ident = find_symbol(table, ident_name);
        if (ident == NULL) {
            printf("ERROR: trying to return '%s', which does not exits.\n", ident_name);
            printf("affected function: %s\n", function->name);
            exit(1);
        }
        enum Symbol_Type ident_type = ident->type;
        enum Symbol_Type ret_type = function->type;
        if (ident_type != ret_type) {
            printf("ERROR: return type does not match return type specified in function header.\n");
            printf("affected function: %s\n", function->name);
            exit(1);
        }
        switch (ident->type) {
        case SYM_BOOL:
            return &(ident->value.bool_val);
        case SYM_INT:
            return &(ident->value.int_val);
        case SYM_STR:
            return &(ident->value.bool_val);
        }
        printf("ERROR: functions are not first class citizens and can therefore not be returned.\n");
        printf("trying to return function '%s'\n", ident_name);
        exit(1);
    case 1:
        // assigning lit
        // TODO: revamp returns to return expressions
        return;
    }
}

void execute_stmt(Symbol_Table *table, Symbol_Table *global_table, AST_Node *stmt, Symbol *function) {
    switch (stmt->type) {
        // ND_ASSIGN, ND_COND, ND_LOOP, ND_RET, EXPR_...
    case ND_ASSIGN:

        return;
    case ND_COND:

        return;
    case ND_LOOP:

        return;
    case ND_RET:
        execute_return(table, stmt, function);
        return;
    default:
        // assume stmt is expr

        return;
    }
}

Symbol *execute_function(Symbol_Table *table, Symbol_Table *global_table, Symbol *function) {
    // TODO: populate table with params
    AST_Node *stmts = function->value.func_val.func_node;
    for (int i = 0; i < stmts->children_size; i += 1) {
        AST_Node *stmt = stmts->children[i];
        execute_stmt(table, global_table, stmt);
    }
}

void interpret_ast(AST_Node *root) {
    Symbol_Table *global_table = create_symbol_table();

    register_funcs(root, global_table);
    Symbol *begin = check_entry_point(global_table);
    execute_function(create_symbol_table(), global_table, begin);
}


int main() {
    // debug parser
#ifdef YYDEBUG
//yydebug = 1;
#endif
// construct AST into `root`
    yyparse();
    // debug AST
    //debug_traverse_tree(root, 0);
    interpret_ast(root);
}
