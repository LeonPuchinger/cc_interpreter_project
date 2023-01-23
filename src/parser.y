%{
#include <stdio.h>
#include <stdlib.h>
#include "../src/types.h"

extern AST_Node *root;

int yylex(void);
void yyerror(const char *);
%}

%define parse.error detailed

%type <ast_node> STMTS STMT ASSIGN FUNC_DEFS FUNC_DEF PARAMS TYPE_ANNOT CONTROL_FLOW COND COND_ALT
%type <ast_node> LOOP RETURN EXPR BOOL_EXPR OP_COMP LIT INT_EXPR OP_NUM STR_EXPR FUNC_CALL EXPRS IDENT

%token tk_assign <str> tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st tk_comp_se tk_add tk_sub tk_concat
%token tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_semicol tk_comma tk_colon tk_ret_tp_ind
%token tk_func_kw tk_loop_kw tk_ret_kw tk_if_kw tk_else_kw tk_str_tp tk_int_tp tk_bool_tp
%token <num> tk_lit_int
%token <str> tk_lit_str tk_lit_bool tk_ident

%union {
    int num;
    char *str;
    struct AST_Node *ast_node;
}

%start START

%%

START: FUNC_DEFS { root = $1; }

FUNC_DEFS: FUNC_DEFS FUNC_DEF {
        if ($1 != NULL) {
            $$ = $1;
        } else {
            $$ = empty_node(ND_FUNC_DEFS);
        }
        add_child($$, $2);
    }
    | %empty { $$ = NULL; }

FUNC_DEF: tk_func_kw IDENT tk_op_paren PARAMS tk_cl_paren tk_ret_tp_ind TYPE_ANNOT tk_op_brace STMTS tk_cl_brace {
    $$ = empty_node(ND_FUNC_DEF);
    add_child($$, $2);
    add_child($$, $4);
    add_child($$, $7);
    add_child($$, $9);
}

PARAMS: IDENT tk_colon TYPE_ANNOT tk_comma PARAMS {
        if ($5 != NULL) {
            $$ = $5;
        } else {
            $$ = empty_node(ND_PARAMS);
        }
        prepend_child($$, $3);
        prepend_child($$, $1);
    }
    | IDENT tk_colon TYPE_ANNOT {
        $$ = empty_node(ND_PARAMS);
        add_child($$, $1);
        add_child($$, $3);
    }
    | %empty { $$ = NULL; }

STMTS: STMTS STMT {
        if ($1 != NULL) {
            $$ = $1;
        } else {
            $$ = empty_node(ND_STMTS);
        }
        add_child($$, $2);
    }
    | %empty { $$ = NULL; }

STMT: ASSIGN tk_semicol { $$ = $1; }
    | CONTROL_FLOW
    | RETURN tk_semicol { $$ = $1; }
    | EXPR tk_semicol { $$ = $1; }

ASSIGN: IDENT tk_assign EXPR {
    $$ = empty_node(ND_ASSIGN);
    add_child($$, $1);
    add_child($$, $3);
}

TYPE_ANNOT: tk_str_tp { $$ = empty_node_st(ND_TYPE, 0); }
    | tk_int_tp { $$ = empty_node_st(ND_TYPE, 1); }
    | tk_bool_tp { $$ = empty_node_st(ND_TYPE, 2); }

CONTROL_FLOW: COND | LOOP

COND: tk_if_kw tk_op_paren BOOL_EXPR tk_cl_paren tk_op_brace STMTS tk_cl_brace COND_ALT { 
    $$ = empty_node(ND_COND);
    add_child($$, $3);
    add_child($$, $6);
    add_child($$, $8);
}

COND_ALT: tk_else_kw tk_op_brace STMTS tk_cl_brace {
        $$ = empty_node(ND_COND_ALT);
        add_child($$, $3);
    }
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

BOOL_EXPR: LIT OP_COMP LIT {
        $$ = empty_node_st(ND_BOOL_EXPR, 0);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | IDENT OP_COMP LIT {
        $$ = empty_node_st(ND_BOOL_EXPR, 1);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | LIT OP_COMP IDENT {
        $$ = empty_node_st(ND_BOOL_EXPR, 2);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | IDENT OP_COMP IDENT {
        $$ = empty_node_st(ND_BOOL_EXPR, 3);
        add_child($$, $1);
        add_child($$, $2);
        add_child($$, $3);
    }
    | tk_lit_bool { $$ = empty_node_st(ND_BOOL_EXPR, 4); add_child($$, str_node($1)); }

OP_COMP: tk_comp_e { $$ = str_node("=="); }
    | tk_comp_ne { $$ = str_node("!="); }
    | tk_comp_gt { $$ = str_node(">"); }
    | tk_comp_ge { $$ = str_node(">="); }
    | tk_comp_st { $$ = str_node("<"); }
    | tk_comp_se { $$ = str_node("<="); }

LIT: tk_lit_bool { $$ = str_node($1); }
    | tk_lit_str { $$ = str_node($1); }
    | tk_lit_int { $$ = int_node($1); }

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
    | tk_lit_int { $$ = empty_node_st(ND_INT_EXPR, 4); add_child($$, int_node($1)); }

OP_NUM: tk_add { $$ = str_node("+"); }
    | tk_sub { $$ = str_node("-"); }

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
    | tk_lit_str { $$ = empty_node_st(ND_STR_EXPR, 4); add_child($$, str_node($1)); }

FUNC_CALL: IDENT tk_op_paren EXPRS tk_cl_paren {
    $$ = empty_node(ND_FUNC_CALL);
    add_child($$, $1);
    add_child($$, $3);
}

EXPRS: EXPRS tk_comma EXPR {
        if ($1 != NULL) {
            $$ = $1;
        } else {
            $$ = empty_node(ND_EXPRS);
        }
        add_child($$, $3);
    }
    | EXPR {
        $$ = empty_node(ND_EXPRS);
        add_child($$, $1);
    }
    | %empty { $$ = NULL; }

IDENT: tk_ident { $$ = str_node($1); }

RETURN: tk_ret_kw EXPR {
        $$ = empty_node(ND_RET);
        add_child($$, $2);
}

%%

void yyerror(const char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
    exit(1);
}
