mkdir -p gen
flex -o gen/lex.yy.c src/lexer.l
bison -d src/parser.y -o gen/parser.tab.c
mkdir -p bin
gcc gen/lex.yy.c gen/parser.tab.c src/interpreter.c -o bin/lang
