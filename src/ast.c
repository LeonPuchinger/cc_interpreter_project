#include <stdlib.h>
#include "types.h"

void init_children(AST_Node *node) {
    // initialize AST_Node children list for `node`
    // use fixed size for now
    // TODO: make dynamic
    node->children = calloc(50, sizeof(AST_Node *));
}

AST_Node *empty_node(AST_Node_Type type) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = type;
    init_children(new);
    return new;
}

AST_Node *int_node(int value) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = ND_INT;
    new->int_value = value;
    init_children(new);
    return new;
}

AST_Node *str_node(char *value) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = ND_STR;
    new->str_value = value;
    init_children(new);
    return new;
}
