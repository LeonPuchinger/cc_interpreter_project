%{
#include <stdio.h>

int yylex(void);
void yyerror(char *);
%}

%token tk_if tk_else tk_for tk_while tk_ret tk_assing tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st
%token tk_comp_se tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_lit_int tk_lit_str tk_ident

%union {
  int num;
  char *str;
}

%start START

%%

START: tk_if { printf("test prod reached\n"); } /* placeholder so this will build */

%%


void yyerror(char *message) {
    extern char *yytext;
    extern int yylineno;
    fprintf(stderr, "ERROR: %s\nline: %d\nat: '%s'\n", message, yylineno, yytext);
}
