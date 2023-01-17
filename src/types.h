#ifndef _TYPES_H
#define _TYPES_H

#include <stdlib.h>

typedef enum {
    ND_ROOT, ND_INT, ND_STR
} AST_Node_Type;

typedef struct _AST_Node {
    int type;
    union {
        int int_value;
        char *str_value;
    };
    struct _AST_Node *children;
} AST_Node;

AST_Node *empty_node(AST_Node_Type type);
AST_Node *int_node(int value);
AST_Node *str_node(char *value);

#endif
