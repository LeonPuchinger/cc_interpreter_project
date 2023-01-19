%{
#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

extern AST_Node *root;

int yylex(void);
void yyerror(char *);
%}

%type <ast_node> STMTS STMT ASSIGN FUNC_DEF PARAMS CONTROL_FLOW COND COND_ALT LOOP EXPR BOOL_EXPR OP_COMP INT_EXPR OP_NUM STR_EXPR FUNC_CALL EXPRS IDENT

%token tk_assign <str> tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st tk_comp_se tk_add tk_sub tk_concat
%token tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_semicol tk_comma
%token tk_func_kw tk_loop_kw tk_ret_kw tk_if_kw tk_else_kw
%token <num> tk_lit_int
%token <str> tk_lit_str tk_lit_bool tk_ident

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

STMT: ASSIGN | FUNC_DEF | CONTROL_FLOW

ASSIGN: IDENT tk_assign EXPR { $$ = empty_node(ND_ASSIGN); $$->children[0] = $1; $$->children[1] = $3; }

FUNC_DEF: tk_func_kw IDENT tk_op_paren PARAMS tk_cl_paren tk_op_brace STMTS tk_cl_brace {
    $$ = empty_node(ND_FUNC_DEF);
    add_child($$, $2);
    add_child($$, $4);
    add_child($$, $7);
}

PARAMS: PARAMS tk_comma IDENT { add_child($1, $3); }
    | IDENT { $$ = empty_node(ND_PARAMS); add_child($$, $1); }
    | %empty { $$ = NULL; }

CONTROL_FLOW: COND | LOOP

COND: tk_if_kw tk_op_paren BOOL_EXPR tk_cl_paren tk_op_brace STMTS tk_cl_brace COND_ALT { 
    $$ = empty_node(ND_COND);
    add_child($$, $3);
    add_child($$, $6);
    add_child($$, $8);
}

COND_ALT: tk_else_kw tk_op_brace STMTS tk_cl_brace { $$ = $3; }
    | %empty { $$ = NULL; }

LOOP: tk_loop_kw tk_op_paren BOOL_EXPR tk_cl_paren tk_op_brace STMTS tk_cl_brace {
    $$ = empty_node(ND_LOOP);
    add_child($$, $3);
    add_child($$, $6);
}

EXPR: BOOL_EXPR
    | INT_EXPR
    | STR_EXPR
    | FUNC_CALL
    | IDENT

BOOL_EXPR: tk_lit_bool OP_COMP tk_lit_bool {
        $$ = empty_node_st(ND_BOOL_EXPR, 0);
        add_child($$, str_node($1));
        add_child($$, $2);
        add_child($$, str_node($3));
    }
    | IDENT OP_COMP tk_lit_bool {
        $$ = empty_node_st(ND_BOOL_EXPR, 1);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, str_node($3));
    }
    | tk_lit_bool OP_COMP IDENT {
        $$ = empty_node_st(ND_BOOL_EXPR, 2);
        add_child($$, str_node($1));
        add_child($$, $2);
        add_child($$, $3);
    }
    | IDENT OP_COMP IDENT {
        $$ = empty_node_st(ND_BOOL_EXPR, 3);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | tk_lit_bool { $$ = empty_node(ND_BOOL_EXPR); add_child($$, str_node($1)); }

OP_COMP: tk_comp_e { $$ = str_node($1); }
    | tk_comp_ne { $$ = str_node($1); }
    | tk_comp_gt { $$ = str_node($1); }
    | tk_comp_ge { $$ = str_node($1); }
    | tk_comp_st { $$ = str_node($1); }
    | tk_comp_se { $$ = str_node($1); }

INT_EXPR: tk_lit_int OP_NUM tk_lit_int {
        $$ = empty_node_st(ND_INT_EXPR, 0);
        add_child($$, int_node($1));
        add_child($$, $2);
        add_child($$, int_node($3));
    }
    | IDENT OP_NUM tk_lit_int {
        $$ = empty_node_st(ND_INT_EXPR, 1);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, int_node($3));
    }
    | tk_lit_int OP_NUM IDENT {
        $$ = empty_node_st(ND_INT_EXPR, 2);
        add_child($$, int_node($1));
        add_child($$, $2);
        add_child($$, $3);
    }
    | IDENT OP_NUM IDENT {
        $$ = empty_node_st(ND_INT_EXPR, 3);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | tk_lit_int { $$ = empty_node(ND_INT_EXPR); add_child($$, int_node($1)); }

OP_NUM: tk_add { $$ = str_node($1); }
    | tk_sub { $$ = str_node($1); }

STR_EXPR: tk_lit_str tk_concat tk_lit_str {
        $$ = empty_node_st(ND_STR_EXPR, 0);
        add_child($$, str_node($1));
        add_child($$, str_node($3));
    }
    | IDENT tk_concat tk_lit_str {
        $$ = empty_node_st(ND_STR_EXPR, 1);
        add_child($$, $1);
        add_child($$, str_node($3));
    }
    | tk_lit_str tk_concat IDENT {
        $$ = empty_node_st(ND_STR_EXPR, 2);
        add_child($$, str_node($1));
        add_child($$, $3);
    }
    | IDENT tk_concat IDENT {
        $$ = empty_node_st(ND_STR_EXPR, 3);
        add_child($$, $1);
        add_child($$, $3);
    }
    | tk_lit_str { $$ = empty_node(ND_INT_EXPR); add_child($$, str_node($1)); }

FUNC_CALL: IDENT tk_op_paren EXPRS tk_cl_paren {
    $$ = empty_node(ND_FUNC_CALL);
    add_child($$, $3);
}

EXPRS: EXPRS tk_comma EXPR { add_child($1, $3); }
    | EXPR { $$ = empty_node(ND_EXPRS); }
    | %empty { $$ = NULL; }

IDENT: tk_ident { $$ = str_node($1); }

%%

void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
