#ifndef _TYPES_H
#define _TYPES_H

typedef enum {
    ND_STMT, ND_INT, ND_STR, ND_ASSIGN, ND_FUNC_DEF, ND_PARAMS, ND_TYPE, ND_COND, ND_LOOP, ND_BOOL_EXPR, ND_STR_EXPR, ND_INT_EXPR, ND_FUNC_CALL, ND_EXPRS,
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

#endif
