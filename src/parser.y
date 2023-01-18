%{
#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

extern AST_Node *root;

int yylex(void);
void yyerror(char *);
%}

%type <ast_node> STMTS STMT ASSIGN EXPR IDENT

%token tk_if tk_else tk_for tk_while tk_ret tk_assign tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st
%token tk_comp_se tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_semicol
%token <num> tk_lit_int
%token <str> tk_lit_str tk_ident

%union {
    int num;
    char *str;
    struct AST_Node *ast_node;
}

%start START

%%

START: STMTS { root = $1; }
STMTS: STMTS STMT tk_semicol { $$ = empty_node(ND_STMT); add_child($1, $$); }
    | %empty { $$ = NULL; }

STMT: ASSIGN

ASSIGN: IDENT tk_assign EXPR { $$ = empty_node(ND_ASSIGN); $$->children[0] = $1; $$->children[1] = $3; }
EXPR: tk_lit_int { $$ = int_node($1); }
    | tk_lit_str { $$ = str_node($1); }

IDENT: tk_ident { $$ = str_node($1); }

%%

void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
