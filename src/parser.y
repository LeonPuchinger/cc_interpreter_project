%{
#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

extern AST_Node *root;

int yylex(void);
void yyerror(char *);
%}

%type <ast_node> STMTS STMT ASSIGN FUNC_DEF CONTROL_FLOW COND COND_ALT LOOP EXPR IDENT

%token tk_assign tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st tk_comp_se 
%token tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_semicol
%token tk_func_kw tk_loop_kw tk_ret_kw tk_if_kw tk_else_kw
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

STMT: ASSIGN FUNC_DEF CONTROL_FLOW

ASSIGN: IDENT tk_assign EXPR { $$ = empty_node(ND_ASSIGN); $$->children[0] = $1; $$->children[1] = $3; }

FUNC_DEF: tk_func_kw IDENT tk_op_paren tk_cl_paren tk_op_brace STMTS tk_cl_brace { $$ = empty_node(ND_FUNC_DEF); add_child($$, $2); /* TODO: params */ add_child($$, NULL); add_child($$, $6); }

CONTROL_FLOW: COND | LOOP

COND: tk_if_kw tk_op_paren tk_cl_paren tk_op_brace STMTS tk_cl_brace COND_ALT { 
    $$ = empty_node(ND_COND);
    /* TODO: bool expr */ add_child($$, NULL);
    add_child($$, $5);
    add_child($$, $7);
}
COND_ALT: tk_else_kw tk_op_brace STMTS tk_cl_brace { $$ = $3; }
    | %empty { $$ = NULL; }

LOOP: tk_loop_kw tk_op_paren tk_cl_paren tk_op_brace STMTS tk_cl_brace {
    $$ = empty_node(ND_LOOP);
    /* TODO: bool expr */ add_child($$, NULL);
    add_child($$, $5);
}

EXPR: tk_lit_int { $$ = int_node($1); }
    | tk_lit_str { $$ = str_node($1); }
    | IDENT

IDENT: tk_ident { $$ = str_node($1); }

%%

void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
