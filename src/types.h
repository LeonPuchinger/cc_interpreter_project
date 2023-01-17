#ifndef _TYPES_H
#define _TYPES_H

typedef enum {
    ND_ROOT
} AST_Node_Type;

typedef struct _AST_Node {
    int type;
    union {
        int int_value;
        char *str_value;
    };
    struct _AST_Node *children;
} AST_Node;

#endif
