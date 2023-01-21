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

enum Symbol_Type { SYM_INT, SYM_BOOL, SYM_STR };

typedef struct Symbol {
    char *name;
    enum Symbol_Type type;
    union {
        int int_val;
        int bool_val;
        char *string_val;
    } value;
} Symbol;

typedef struct Scope {
    Symbol **symbols;
    int num_symbols;
    struct Scope *parent;
} Scope;

typedef struct SymbolTable {
    Scope *current_scope;
} SymbolTable;

Scope *create_scope(Scope *parent);
void push_scope(SymbolTable *table);
void pop_scope(SymbolTable *table);
Symbol *find_symbol(Scope *scope, char *name);
void set_symbol(SymbolTable *table, char *name, enum Symbol_Type type, void *value);

#endif
