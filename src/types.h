#ifndef _TYPES_H
#define _TYPES_H

// AST

typedef enum {
    ND_FUNC_DEFS, ND_FUNC_DEF, ND_STMTS, ND_INT, ND_STR, ND_ASSIGN, ND_PARAMS, ND_TYPE, ND_COND, ND_LOOP, ND_BOOL_EXPR, ND_STR_EXPR, ND_INT_EXPR, ND_FUNC_CALL, ND_EXPRS,
} AST_Node_Type;

typedef char AST_Node_Subtype;

typedef struct AST_Node {
    AST_Node_Type type;
    AST_Node_Subtype subtype;
    union {
        int int_value;
        char *str_value;
    };
    int children_size, children_buffer_size;
    struct AST_Node **children;
} AST_Node;

AST_Node *empty_node(AST_Node_Type type);
AST_Node *empty_node_st(AST_Node_Type type, AST_Node_Subtype subtype);
AST_Node *int_node(int value);
AST_Node *str_node(char *value);

void add_child(AST_Node *node, AST_Node *child);
void prepend_child(AST_Node *node, AST_Node *child);

// Symbol table

enum Symbol_Type { SYM_INT, SYM_BOOL, SYM_STR, SYM_FUNC };

typedef struct Symbol {
    char *name;
    enum Symbol_Type type;
    union {
        int int_val;
        int bool_val;
        char *string_val;
        struct {
            char **param_names;
            enum Symbol_Type *param_types;
            int num_params;
            enum Symbol_Type return_type;
            struct AST_Node *func_node;
        } func_val;
    } value;
} Symbol;

typedef struct Scope {
    Symbol **symbols;
    int num_symbols;
    struct Scope *parent;
} Scope;

typedef struct Symbol_Table {
    Scope *current_scope;
} Symbol_Table;

Symbol_Table *create_symbol_table();
Scope *create_scope(Scope *parent);
void push_scope(Symbol_Table *table);
void pop_scope(Symbol_Table *table);
Symbol *find_symbol(Scope *scope, char *name);
void set_symbol(Symbol_Table *table, char *name, enum Symbol_Type type, void *value, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node);
void set_symbol_int(Symbol_Table *table, char *name, int value);
void set_symbol_bool(Symbol_Table *table, char *name, int value);
void set_symbol_string(Symbol_Table *table, char *name, char *value);
void set_symbol_func(Symbol_Table *table, char *name, char **param_names, enum Symbol_Type *param_types, int num_params, enum Symbol_Type return_type, struct AST_Node *func_node);

#endif
