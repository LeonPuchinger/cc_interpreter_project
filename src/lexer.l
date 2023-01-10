%{
    /* options */
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

{IF} { return if_kw; }
{ELSE} { return else_kw; }
{FOR} { return for_kw; }
{WHILE} { return while_kw; }
{RET} { return ret_kw; }
{ASSIGN} { return assign; }
{COMP_E} { return comp_e; }
{COMP_NE} { return comp_ne; }
{COMP_GT} { return comp_gt; }
{COMP_GE} { return comp_ge; }
{COMP_ST} { return comp_st; }
{COMP_SE} { return comp_se; }
{OP_PAREN} { return op_paren; }
{CL_PAREN} { return cl_paren; }
{OP_BRACE} { return op_brace; }
{CL_BRACE} { return cl_brace; }
{LIT_INT} { yylval.num = atoi(yytext); return int_lit; } 
{LIT_STR} { return str_lit; }
{IDENT} { return ident; }

%%
