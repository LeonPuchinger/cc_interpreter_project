%{
#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

int yylex(void);
void yyerror(char *);
void interpret_ast(AST_Node *);
%}

%type <ast_node> START

%token tk_if tk_else tk_for tk_while tk_ret tk_assing tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st
%token tk_comp_se tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_lit_int tk_lit_str tk_ident

%union {
    int num;
    char *str;
    struct AST_Node *ast_node;
}

%start START

%%

START: tk_if { interpret_ast(int_node(1)); }


%%

void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
