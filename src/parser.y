%{
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    ND_ROOT, ND_INT, ND_STR
} AST_Node_Type;

typedef struct AST_Node {
    AST_Node_Type type;
    union {
        int int_value;
        char *str_value;
    };
    struct AST_Node *children;
} AST_Node;

AST_Node *empty_node(AST_Node_Type type);
AST_Node *int_node(int value);
AST_Node *str_node(char *value);

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

START: tk_if { $$ = int_node(1); }


%%

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

void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
