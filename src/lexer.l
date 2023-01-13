%{
#include <stdio.h>
#include "gen/prolang.tab.h"
%}

/* keywords */
IF "if"
ELSE "else"
FOR "for"
WHILE "while"
RET "return"
/* comparison operators */
ASSIGN "="
COMP_E "=="
COMP_NE "!="
COMP_GT ">"
COMP_GE ">="
COMP_ST "<"
COMP_SE "<="
/* structural components */
OP_PAREN "("
CL_PAREN ")"
OP_BRACE "{"
CL_BRACE "}"
/* literals */
LIT_INT [0-9]+
LIT_STR \"[^\"]*\"
/* identifier */
IDENT [a-zA-Z][_a-zA-Z0-9]*

%%

{IF} { return tk_if; }
{ELSE} { return tk_else; }
{FOR} { return tk_for; }
{WHILE} { return tk_while; }
{RET} { return tk_ret; }
{ASSIGN} { return tk_assing; }
{COMP_E} { return tk_comp_e; }
{COMP_NE} { return tk_comp_ne; }
{COMP_GT} { return tk_comp_gt; }
{COMP_GE} { return tk_comp_ge; }
{COMP_ST} { return tk_comp_st; }
{COMP_SE} { return tk_comp_se; }
{OP_PAREN} { return tk_op_paren; }
{CL_PAREN} { return tk_cl_paren; }
{OP_BRACE} { return tk_op_brace; }
{CL_BRACE} { return tk_cl_brace; }
{LIT_INT} { yylval.num = atoi(yytext); return tk_lit_int; }
{LIT_STR} { yylval.str = strdup(yytext); return tk_lit_str; }
{IDENT} { return tk_ident; }

%%

int yywrap() {
    // only read one file
    return 1;
}

int main (int argc, char **argv) {
    yyin = fopen(argv[1], "r");
    yylex();
}
