#include <stdio.h>
#include <stdlib.h>
#include "../gen/parser.tab.h"
#include "types.h"
#include "debug.h"

AST_Node *root;

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

void interpret_ast(AST_Node *root) {
    Symbol_Table *table = create_symbol_table();

    register_funcs(root, table);
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
