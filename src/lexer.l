%{
#include <stdio.h>
#include <stdlib.h>
#include "../gen/parser.tab.h"

char *trim_segment(const char *token, int token_length);
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
"->" { return tk_ret_tp_ind; }
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
"true" { yylval.str = strdup(yytext); return tk_lit_bool; }
"false" { yylval.str = strdup(yytext); return tk_lit_bool; }
[0-9]+ { yylval.num = atoi(yytext); return tk_lit_int; }
\"[^\"]*\" { yylval.str = trim_segment(yytext + 1, yyleng - 2); return tk_lit_str; }
[a-zA-Z][_a-zA-Z0-9]* { yylval.str = strdup(yytext); return tk_ident; }
. { printf("could not match token: %s", yytext); }

%%

char *trim_segment(const char *token, int token_length) {
    // trim input string `token` by `token_length`
    char *duplicate = malloc(token_length + 1);
    memcpy(duplicate, token, token_length);
    duplicate[token_length] = 0;
    return duplicate;
}
