#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../gen/parser.tab.h"
#include "types.h"
#include "debug.h"

AST_Node *root;

enum Symbol_Type subtype_to_symbol_type(int subtype);
void register_funcs(AST_Node *root, Symbol_Table *table);
Symbol *check_entry_point(Symbol_Table *table);
void execute_assign(Symbol_Table *table, Symbol_Table *global_table, AST_Node *assign, Symbol *function);
Symbol *execute_stmt(Symbol_Table *table, Symbol_Table *global_table, AST_Node *stmt, Symbol *function);
Symbol *execute_return(Symbol_Table *table, Symbol_Table *global_table, AST_Node *ret, Symbol *function);
Symbol *execute_int_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function);
Symbol *execute_str_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function);
Symbol *execute_bool_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function);
Symbol *execute_expression(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function);
Symbol *call_function(Symbol_Table *table, Symbol_Table *global_table, AST_Node *func_call, Symbol *source_function);
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

void execute_assign(Symbol_Table *table, Symbol_Table *global_table, AST_Node *assign, Symbol *function) {
    char *variable_name = assign->children[0]->str_value;
    if (find_symbol(global_table, variable_name) != NULL) {
        printf("ERROR: cannot reassign functions (%s).\n", variable_name);
        exit(1);
    }
    AST_Node *expr = assign->children[1];
    Symbol *existing = find_symbol(table, variable_name);
    Symbol *result = execute_expression(table, global_table, expr, function);
    if (existing != NULL) {
        // symbol already exits, check if types are compatible
        if (existing->type != result->type) {
            printf("ERROR: trying to assign an expression to a variable ('%s') of different type.\n", variable_name);
            printf("affected function: %s\n", function->name);
            exit(1);
        }
    }
    result->name = variable_name;
    set_existing_symbol(table, result);
}

Symbol *execute_return(Symbol_Table *table, Symbol_Table *global_table, AST_Node *ret, Symbol *function) {
    AST_Node *expr = ret->children[0];
    Symbol *result = execute_expression(table, global_table, expr, function);
    // check whether the return type matches what is actually returned
    enum Symbol_Type ident_type = result->type;
    enum Symbol_Type ret_type = function->value.func_val.return_type;
    if (ident_type != ret_type) {
        printf("ERROR: return type does not match return type specified in function header.\n");
        printf("affected function: %s\n", function->name);
        exit(1);
    }
    return result;
}

Symbol *execute_int_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function) {
    if (expr->children_size == 1) {
        // expression is just an int literal
        return create_symbol_int("", expr->children[0]->int_value);
    }
    AST_Node *lhs_expr = expr->children[0];
    int lhs_value = 0;
    AST_Node *rhs_expr = expr->children[2];
    int rhs_value = 0;
    if (lhs_expr->type == ND_INT) {
        // left side of expr is a literal => just read value
        lhs_value = lhs_expr->int_value;
    }
    if (lhs_expr->type == ND_STR) {
        // left side of expr is an ident => evaluate
        Symbol *result = execute_expression(table, global_table, lhs_expr, function);
        lhs_value = result->value.int_val;
    }
    if (rhs_expr->type == ND_INT) {
        // right side of expr is a literal => just read value
        rhs_value = rhs_expr->int_value;
    }
    if (rhs_expr->type == ND_STR) {
        // right side of expr is an ident => evaluate
        Symbol *result = execute_expression(table, global_table, rhs_expr, function);
        rhs_value = result->value.int_val;
    }
    // calculate result of the expression
    char *operator = expr->children[1]->str_value;
    int cal_result = 0;
    if (strcmp(operator, "+") == 0) {
        cal_result = lhs_value + rhs_value;
    } else {
        cal_result = lhs_value - rhs_value;
    }
    return create_symbol_int("", cal_result);
}

Symbol *execute_str_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function) {
    if (expr->children_size == 1) {
        // expression is just a str literal
        return create_symbol_string("", expr->children[0]->str_value);
    }
    AST_Node *lhs_expr = expr->children[0];
    char *lhs_value;
    AST_Node *rhs_expr = expr->children[1];
    char *rhs_value;
    if (expr->subtype == 0 || expr->subtype == 2) {
        // left side of expr is a literal => just read value
        lhs_value = lhs_expr->str_value;
    }
    if (expr->subtype == 1 || expr->subtype == 3) {
        // left side of expr is an ident => evaluate
        Symbol *result = execute_expression(table, global_table, lhs_expr, function);
        lhs_value = result->value.string_val;
    }
    if (expr->subtype == 0 || expr->subtype == 1) {
        // right side of expr is a literal => just read value
        rhs_value = rhs_expr->str_value;
    }
    if (expr->subtype == 2 || expr->subtype == 3) {
        // right side of expr is an ident => evaluate
        Symbol *result = execute_expression(table, global_table, rhs_expr, function);
        rhs_value = result->value.string_val;
    }
    // always concatenate (no other operator)
    char *concat_result = strcat(lhs_value, rhs_value);
    return create_symbol_string("", concat_result);
}

Symbol *execute_bool_expr(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function) {
    if (expr->children_size == 1) {
        // expression is just a bool literal
        char* value = expr->children[0]->str_value;
        if (strcmp(value, "true") == 0) {
            return create_symbol_bool("", 1);
        }
        return create_symbol_bool("", 0);
    }
    AST_Node *lhs_expr = expr->children[0];
    Symbol *lhs_symbol = NULL;
    AST_Node *rhs_expr = expr->children[2];
    Symbol *rhs_symbol = NULL;
    if (expr->subtype == 0 || expr->subtype == 2) {
        // left side of expr is a literal => evaluate
        AST_Node *temp;
        // check whether the literal is int or bool
        if (lhs_expr->str_value != NULL) {
            temp = empty_node(ND_BOOL_EXPR);
        } else {
            temp = empty_node(ND_INT_EXPR);
        }
        add_child(temp, lhs_expr);
        lhs_symbol = execute_expression(table, global_table, temp, function);
    }
    if (expr->subtype == 1 || expr->subtype == 3) {
        // left side of expr is an ident => evaluate
        lhs_symbol = execute_expression(table, global_table, lhs_expr, function);
    }
    if (expr->subtype == 0 || expr->subtype == 1) {
        // right side of expr is a literal => evaluate
        AST_Node *temp;
        // check whether the literal is int or bool
        if (rhs_expr->str_value != NULL) {
            temp = empty_node(ND_BOOL_EXPR);
        } else {
            temp = empty_node(ND_INT_EXPR);
        }
        add_child(temp, rhs_expr);
        rhs_symbol = execute_expression(table, global_table, temp, function);
    }
    if (expr->subtype == 2 || expr->subtype == 3) {
        // right side of expr is an ident => evaluate
        Symbol *rhs_symbol = execute_expression(table, global_table, rhs_expr, function);
    }
    // check if types can be compared
    if (lhs_symbol->type != rhs_symbol->type) {
        printf("ERROR: comparing two different types (int and bool).\n");
        printf("affected function: %s\n", function->name);
        exit(1);
    }
    // calculate result of the expression
    char *operator = expr->children[1]->str_value;
    // extract values based on data type of the comparison
    int lhs_value;
    int rhs_value;
    if (lhs_symbol->type == SYM_BOOL) {
        lhs_value = lhs_symbol->value.bool_val;
        rhs_value = rhs_symbol->value.bool_val;
    } else {
        lhs_value = lhs_symbol->value.int_val;
        rhs_value = rhs_symbol->value.int_val;
    }
    int comp_result = 0;
    if (strcmp(operator, "==") == 0) {
        comp_result = lhs_value == rhs_value;
    }
    if (strcmp(operator, "!=") == 0) {
        comp_result = lhs_value != rhs_value;
    }
    if (strcmp(operator, "<") == 0) {
        comp_result = lhs_value < rhs_value;
    }
    if (strcmp(operator, "<=") == 0) {
        comp_result = lhs_value <= rhs_value;
    }
    if (strcmp(operator, ">") == 0) {
        comp_result = lhs_value > rhs_value;
    }
    if (strcmp(operator, ">=") == 0) {
        comp_result = lhs_value >= rhs_value;
    }
    return create_symbol_bool("", comp_result);
}

Symbol *execute_expression(Symbol_Table *table, Symbol_Table *global_table, AST_Node *expr, Symbol *function) {
    switch (expr->type) {
    case ND_STR:
        // expression is an ident
        char *ident_name = expr->str_value;
        if (find_symbol(global_table, ident_name)) {
            // ident can't have the same name as an already existent function
            printf("ERROR: function '%s' cannot be accesses as a variable. functions are not first class citizens.\n", ident_name);
            printf("affected function: %s\n", function->name);
            exit(1);
        }
        Symbol *ident = find_symbol(table, ident_name);
        if (ident == NULL) {
            // ident does not exits
            printf("ERROR: trying to access '%s', which does not exits.\n", ident_name);
            printf("affected function: %s\n", function->name);
            exit(1);
        }
        // build a new symbol for the identifier
        switch (ident->type) {
        case SYM_BOOL:
            return create_symbol_bool("", ident->value.bool_val);
        case SYM_INT:
            return create_symbol_int("", ident->value.int_val);
        case SYM_STR:
            return create_symbol_string("", ident->value.string_val);
        }
        printf("ERROR: cannot use function in an expression that is not a function call.\n");
        printf("affected function: %s\n", function->name);
        exit(1);
    case ND_INT_EXPR:
        return execute_int_expr(table, global_table, expr, function);
    case ND_STR_EXPR:
        return execute_str_expr(table, global_table, expr, function);
    case ND_BOOL_EXPR:
        return execute_bool_expr(table, global_table, expr, function);
    case ND_FUNC_CALL:
        return call_function(table, global_table, expr, function);
    }
}

Symbol *execute_stmt(Symbol_Table *table, Symbol_Table *global_table, AST_Node *stmt, Symbol *function) {
    switch (stmt->type) {
        // ND_ASSIGN, ND_COND, ND_LOOP, ND_RET, EXPR_...
    case ND_ASSIGN:
        execute_assign(table, global_table, stmt, function);
        return NULL;
    case ND_COND:

        return NULL;
    case ND_LOOP:

        return NULL;
    case ND_RET:
        return execute_return(table, global_table, stmt, function);
    default:
        // assume stmt is expr
        execute_expression(table, global_table, stmt, function);
        return NULL;
    }
}

Symbol *call_function(Symbol_Table *table, Symbol_Table *global_table, AST_Node *func_call, Symbol *source_function) {
    char *dest_func_name = func_call->children[0]->str_value;
    // TODO: check if stdlib function
    Symbol *dest_function = find_symbol(global_table, dest_func_name);
    if (dest_function == NULL) {
        printf("ERROR: called function does not exist: '%s'.\n", dest_func_name);
        printf("affected function: %s\n", source_function->name);
        exit(1);
    }
    // compare function param types with called types and populate symbol table with params
    int expr_size = 0;
    AST_Node *exprs = NULL;
    if (func_call->children_size == 2) {
        // function called with parameters
        exprs = func_call->children[1];
        expr_size = exprs->children_size;
    }
    if (dest_function->value.func_val.num_params != expr_size) {
        printf("ERROR: function %s called with incorrect number of arguments\n", dest_function->name);
        printf("affected function: %s\n", source_function->name);
        exit(1);
    }
    // check whether the function call uses the right param types
    // if so, populate the symbol table for the called function with the parameter
    Symbol_Table *new_table = create_symbol_table();
    for (int i = 0; i < dest_function->value.func_val.num_params; i += 1) {
        // iterate over exprs in the func call and params defined in the function
        enum Symbol_Type param_type = dest_function->value.func_val.param_types[i];
        Symbol *expr_symbol = execute_expression(table, global_table, exprs->children[i], source_function);
        if (param_type != expr_symbol->type) {
            printf("ERROR: type mismatch when calling function %s\n.", dest_func_name);
            printf("affected function: %s\n", source_function->name);
            exit(1);
        }
        expr_symbol->name = dest_function->value.func_val.param_names[i];
        set_existing_symbol(new_table, expr_symbol);
    }
    execute_function(new_table, global_table, dest_function);
}

Symbol *execute_function(Symbol_Table *table, Symbol_Table *global_table, Symbol *function) {
    // assume `table` is already populated with function params
    AST_Node *stmts = function->value.func_val.func_node;
    for (int i = 0; i < stmts->children_size; i += 1) {
        AST_Node *stmt = stmts->children[i];
        Symbol *return_result = execute_stmt(table, global_table, stmt, function);
        if (return_result != NULL) {
            // function has hit return statement
            return return_result;
        }
    }
}

void interpret_ast(AST_Node *root) {
    Symbol_Table *global_table = create_symbol_table();

    register_funcs(root, global_table);
    Symbol *begin = check_entry_point(global_table);
    Symbol *rc = execute_function(create_symbol_table(), global_table, begin);
    if (rc != NULL) {
        // make the interpreter return whatever the begin function returns as the exit code
        exit(rc->value.int_val);
    }
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
