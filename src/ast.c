#include <stdlib.h>
#include "types.h"

AST_Node *empty_node(AST_Node_Type type) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = type;
    return new;
}

AST_Node *int_node(int value) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = ND_INT;
    new->int_value = value;
    return new;
}

AST_Node *str_node(char *value) {
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = ND_STR;
    new->str_value = value;
    return new;
}
