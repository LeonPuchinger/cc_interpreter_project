%{
    // definitions
%}

%x keywords

IF "if"
ELSE "else"
FOR "for"
WHILE "while"
RET "return"

%x comparison_operators

ASSIGN "="
COMP_E "=="
COMP_NE "!="
COMP_GT ">"
COMP_GE ">="
COMP_ST "<"
COMP_SE "<="

%x structural components

OP_PAREN "("
CL_PAREN ")"
OP_BRACE "{"
CL_BRACE "}"

%x literals

LIT_INT [0-9]+
LIT_STR \"[^\"]*\"

%x identifier

IDENT [a-zA-Z][_a-zA-Z0-9]*


%%
    
%%
