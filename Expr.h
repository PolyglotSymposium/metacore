#ifndef __EXPR_H__
#define __EXPR_H__

#include "Cons.h"

typedef enum Func
{
 fADD, fMULT, fKCOMB, fICOMB, fBCOMB
} Func;

typedef enum ExprTag
{
 eINT, eAPPLY, eSTRING, eFUN
} ExprTag;

typedef struct Expr {
  union {
    struct {
      struct Expr* applyF;
      Cons* args;
    };
    Func func;
    int intValue;
    char* cString;
  };
  ExprTag type;
} Expr;

Expr* num(int);
Expr* str(char*);
Expr* ap(Expr*, Cons*);
Expr* fun(Func);

#endif /* __EXPR_H__ */
