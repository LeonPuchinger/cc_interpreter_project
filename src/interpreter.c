#include <stdio.h>
#include "../gen/parser.tab.h"
#include "types.h"
#include "debug.h"

AST_Node *root;

void interpret_ast(AST_Node *root) {
    Symbol_Table *table = create_symbol_table();
}

int main() {
    // debug parser
    #ifdef YYDEBUG
    //yydebug = 1;
    #endif
    // construct AST into `root`
    yyparse();
    // debug AST
    //debug_traverse_tree(root, 0);
    interpret_ast(root);
}
