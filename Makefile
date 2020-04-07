# Depends on the Boehm GC:
#
#     sudo apt install libgc-dev

FILES		= Cons.c Lexer.c Parser.c Expr.c Eval.c main.c
CC			= g++
CFLAGS	= -Wall -g -ansi -DYY_NO_UNPUT=1

test: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o test -lgc

Lexer.c: Lexer.l
	flex Lexer.l

Parser.c:	Parser.y Lexer.c
	bison Parser.y

clean:
	rm -f *.o Lexer.c Lexer.h Parser.c Parser.h test