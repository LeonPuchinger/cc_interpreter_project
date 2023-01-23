#!/bin/bash
# tested with the following versions:
# flex: 2.6.4
# bison: 3.7.6
# gcc: 11.3.1
mkdir -p gen
flex -o gen/lex.yy.c --header-file=gen/lex.yy.h src/lexer.l
bison -d src/parser.y -o gen/parser.tab.c
mkdir -p bin
gcc gen/lex.yy.c gen/parser.tab.c src/ast.c src/symbol_table.c src/interpreter.c -o bin/lang
