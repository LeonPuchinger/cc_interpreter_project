#include <stdio.h>
#include "../gen/parser.tab.h"
#include "types.h"

AST_Node *root;

void interpret_ast(AST_Node *root) {
    // TODO
}

int main() {
    yyparse();
    printf("%s\n", root->str_value);
}
