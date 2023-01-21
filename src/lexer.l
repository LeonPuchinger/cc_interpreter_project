%{
#include <stdio.h>
#include "../gen/parser.tab.h"
%}

%option noinput nounput noyywrap yylineno

%%

"//".* /* skip single line comments */
[ \n\r\t]+ /* skip whitespace */
"if" { return tk_if_kw; }
"else" { return tk_else_kw; }
"loop" { return tk_loop_kw; }
"return" { return tk_ret_kw; }
"fun" { return tk_func_kw; }
"str" { return tk_str_tp; }
"int" { return tk_int_tp; }
"bool" { return tk_bool_tp; }
"==" { return tk_comp_e; }
"=" { return tk_assign; }
"!=" { return tk_comp_ne; }
">=" { return tk_comp_ge; }
">" { return tk_comp_gt; }
"<=" { return tk_comp_se; }
"<" { return tk_comp_st; }
"++" { return tk_concat; }
"+" { return tk_add; }
"-" { return tk_sub; }
"(" { return tk_op_paren; }
")" { return tk_cl_paren; }
"{" { return tk_op_brace; }
"}" { return tk_cl_brace; }
";" { return tk_semicol; }
":" { return tk_colon; }
"," { return tk_comma; }
[0-9]+ { yylval.num = atoi(yytext); return tk_lit_int; }
\"[^\"]*\" { yylval.str = strdup(yytext); return tk_lit_str; }
"true" | "false" { yylval.str = strdup(yytext); return tk_lit_bool; }
[a-zA-Z][_a-zA-Z0-9]* { return tk_ident; }
. { printf("could not match token: %s", yytext); }

%%
