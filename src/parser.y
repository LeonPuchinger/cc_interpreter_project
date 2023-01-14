%{

%}

%token tk_if tk_else tk_for tk_while tk_ret tk_assing tk_comp_e tk_comp_ne tk_comp_gt tk_comp_ge tk_comp_st
%token tk_comp_se tk_op_paren tk_cl_paren tk_op_brace tk_cl_brace tk_lit_int tk_lit_str tk_ident

%start start

%%

start: tk_if /* placeholder so this will build */

%%
