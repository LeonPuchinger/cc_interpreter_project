#include <stdlib.h>
#include "types.h"

#define AST_CHILDREN_BUFFER_SIZE 10

void init_children(AST_Node *node) {
    // Initialize AST_Node children list for `node`
    node->children_size = 0;
    node->children_buffer_size = AST_CHILDREN_BUFFER_SIZE;
    node->children = calloc(AST_CHILDREN_BUFFER_SIZE, sizeof(AST_Node *));
}

void add_child(AST_Node *node, AST_Node *child) {
    // Add a child to `node`. If there is not enough space, reallocate more space.
    if (node->children_size >= node->children_buffer_size) {
        node->children_buffer_size += AST_CHILDREN_BUFFER_SIZE;
        node->children = realloc(node->children, node->children_buffer_size);
    }
    node->children[node->children_size] = child;
    node->children_size += 1;
}

AST_Node *empty_node(AST_Node_Type type) {
    // Create a AST node with only a AST node type.
    // The subtype is NULL per default, as it is
    // only needed in special situations
    // The node does not yet have any value or children.
    AST_Node *new = calloc(1, sizeof(AST_Node));
    new->type = type;
    new->subtype = -1;
    init_children(new);
    return new;
}

AST_Node *empty_node_st(AST_Node_Type type, AST_Node_Subtype subtype) {
    // Shortcut to initialize an empty node with both a type and subtype.
    AST_Node *new = empty_node(type);
    new->subtype = subtype;
    return new;
}

AST_Node *int_node(int value) {
    // Shortcut to initialize a node of type ND_INT with a value.
    AST_Node *new = empty_node(ND_INT);
    new->int_value = value;
    return new;
}

AST_Node *str_node(char *value) {
    // Shortcut to initialize a node of type ND_STR with a value.
    AST_Node *new = empty_node(ND_STR);
    new->str_value = value;
    return new;
}
