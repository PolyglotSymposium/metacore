%{

#include "Symbol.h"
#include "Struct.h"
#include "Nat.h"
#include "Str.h"
#include "Pair.h"
#include "SymbolValue.h"
#include "StructValue.h"
#include "Parser.h"
#include "Lexer.h"

int yyerror(Struct** expr, yyscan_t scanner, const char* msg) {
  fprintf(stderr, "Parser: %s\n", msg);
  exit(1);
  return 0;
}

%}

%code requires {
  typedef void* yyscan_t;
}

%output  "Parser.c"
%defines "Parser.h"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { Struct** expr }
%parse-param { yyscan_t scanner }

%union {
  int value;
  char* string;
  Symbol name;
  Struct* expr;
  Struct* apply;
}

%token          TOKEN_LPAREN "("
%token          TOKEN_RPAREN ")"
%token          TOKEN_LSQBRK "["
%token          TOKEN_RSQBRK "]"
%token <value>  TOKEN_NUMBER "number"
%token <string> TOKEN_STRING "string"
%token <name>   TOKEN_NAME   "name"

%type <expr>  expr
%type <apply> apply

%%

input
: expr { *expr = $1; }
;

expr
: TOKEN_LPAREN apply[A] TOKEN_RPAREN                  { $$ = $A; }
| TOKEN_LSQBRK TOKEN_NAME[N] apply[A] TOKEN_RSQBRK    { $$ = structFromNameAndPairs($N, $A); }
| TOKEN_LSQBRK TOKEN_NAME[N] expr[E] TOKEN_RSQBRK     { $$ = structFromNameAnd1($N, $E); }
| TOKEN_LSQBRK TOKEN_NAME[N] TOKEN_RSQBRK             { $$ = structFromName($N); }
| TOKEN_NUMBER                                        { $$ = newNat($1); }
| TOKEN_STRING                                        { $$ = newStr($1); }
| TOKEN_NAME                                          { $$ = newSymbol($1); }
;

apply
: apply[H] expr[T] { $$ = newPair($H, $T); }
| expr[H] expr[T] { $$ = newPair($H, $T); }
;

%%
