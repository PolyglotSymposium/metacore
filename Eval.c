#include "Eval.h"
#include "Primitives.h"
#include "Str.h"
#include "Nat.h"
#include "Error.h"
#include "PrimFun.h"
#include "Pair.h"
#include "StructValue.h"
#include "SymbolValue.h"
#include "Forms.h"
#include <gc.h>
#include <assert.h>
#include <stdio.h>

Struct* eval(Map*, Struct*);

Struct* evalByLookup(Map* env, Struct* e) {
  Symbol s = asSymbol(e);
  Struct* v = matchPrim(s);
  if (v == NULL) {
    v = lookup(s, env);
    if (v == NULL) {
      v = undefined(s);
    }
  }
  return v;
}

Struct* evalByApply(Map* env, Struct* e) {
  Struct* v = apply(eval(env, asFirst(e)), eval(env, asSecond(e)));
  assert(v != NULL);
  return v;
}

Struct* eval(Map* env, Struct* e) {
  assert(e != NULL);
  Struct* v = NULL;
  switch (get_tag(e)) {
  case NAT_SYMBOL     :
  case STR_SYMBOL     :
  case PRIMFUN1_SYMBOL:
  case PRIMFUN2_SYMBOL:
  case CLOHALF_SYMBOL :
  case PRIMFUN3_SYMBOL:
  case CLOTHIRD_SYMBOL:
  case TWOTHIRD_SYMBOL:
  case ERROR_SYMBOL   : v = e                    ; break;
  case PAIR_SYMBOL    : v = evalByApply(env, e)  ; break;
  case STRUCT_SYMBOL  : v = matchForm(dequote(e)); break;
  case SYMBOL_SYMBOL  : v = evalByLookup(env, e) ; break;
  default             :
    fprintf(stderr, "Unknown expression type: %s\n", decompressSymbol(get_tag(e)));
    int UNHANDLED_EXPR_TAG = 0;
    assert(UNHANDLED_EXPR_TAG);
  }
  return v;
}


Struct* evalTopLevel(Map* env, Map* envOut, Struct* e) {
  assert(envOut == NULL);
  Struct* out = NULL;
  switch (get_tag(e)) {
  case STRUCT_SYMBOL: envOut = matchTopLevel(env, dequote(e)); break;
  default:                                                     break;
  }
  if (envOut == NULL) {
    out = eval(env, e);
    envOut = env;
  }
  return out;
}
