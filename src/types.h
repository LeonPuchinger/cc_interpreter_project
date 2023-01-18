#ifndef _TYPES_H
#define _TYPES_H

typedef enum {
    ND_STMT, ND_INT, ND_STR, ND_ASSIGN,
} AST_Node_Type;

typedef struct AST_Node {
    AST_Node_Type type;
    union {
        int int_value;
        char *str_value;
    };
    int children_size, children_buffer_size;
    struct AST_Node **children;
} AST_Node;

AST_Node *empty_node(AST_Node_Type type);
AST_Node *int_node(int value);
AST_Node *str_node(char *value);

void add_child(AST_Node *node, AST_Node *child);

#endif
