#include <stdlib.h>
#include <gc.h>
#include <assert.h>
#include <string.h>
#include "Primitives.h"
#include "Str.h"
#include "Nat.h"
#include "Error.h"
#include "PrimFun.h"
#include "Pair.h"
#include "StructValue.h"
#include "SymbolValue.h"
#include "StructParser.h"

static Struct* kcomb(Struct* x, Struct* _) {
  return x;
}

static Struct* ckcomb(Struct* _, Struct* x) {
  return x;
}

static Struct* type_of(Struct* x) {
  Struct* t = NULL;
  Symbol type = get_tag(x);
  if (type == ERROR_SYMBOL)
       { t = x; }
  else { t = newSymbol(type); }
  return t;
}

static Struct* require(Symbol tag, Struct* x) {
  Struct* v = NULL;
  Symbol actualTag = get_tag(x);
  if (actualTag != tag) {
    if (actualTag == ERROR_SYMBOL) {
      v = x;
    } else {
      v = typeError(tag, actualTag);
    }
  }
  return v;
}

static Struct* pair(Struct* x, Struct* y) {
  Struct* v = NULL;
  if (get_tag(x) == ERROR_SYMBOL) {
    v = x;
  } else if (get_tag(y) == ERROR_SYMBOL) {
    v = y;
  } else {
    v = newPair(x, y);
  }
  return v;
}

static Struct* parse(Struct* x) {
  Struct* v = require(STR_SYMBOL, x);
  if (v == NULL) {
    v = chars_to_struct(asStr(x));
    if (v == NULL) {
      // TODO define Maybe distinctly
      v = atomic_struct(6887296461 /* nothing */);
    } else {
      v = singleton_struct(641673 /* just */, v);
    }
    v = quote(v);
  }
  return v;

}

static Struct* fst(Struct* x) {
  Struct* v = require(PAIR_SYMBOL, x);
  if (v == NULL) {
    v = asFirst(x);
  }
  return v;
}

static Struct* snd(Struct* x) {
  Struct* v = require(PAIR_SYMBOL, x);
  if (v == NULL) {
    v = asSecond(x);
  }
  return v;
}

static Struct* tag_of(Struct* x) {
  Struct* v = require(STRUCT_SYMBOL, x);
  if (v == NULL) {
    v = newSymbol(get_tag(dequote(x)));
  }
  return v;
}

static Struct* struct_size(Struct* x) {
  Struct* v = require(STRUCT_SYMBOL, x);
  if (v == NULL) {
    v = newNat(get_size(dequote(x)));
  }
  return v;
}

static Struct* add(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(NAT_SYMBOL, x);
  if (v == NULL) {
    v = require(NAT_SYMBOL, y);
    if (v == NULL) {
      v = newNat(asNat(x) + asNat(y));
    }
  }
  return v;
}

static Struct* monus(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(NAT_SYMBOL, x);
  if (v == NULL) {
    v = require(NAT_SYMBOL, y);
    if (v == NULL) {
      unsigned long xNat = asNat(x);
      unsigned long yNat = asNat(y);
      if (xNat < yNat) {
        v = newNat(0);
      } else {
        v = newNat(xNat - yNat);
      }
    }
  }
  return v;
}

static Struct* mult(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(NAT_SYMBOL, x);
  if (v == NULL) {
    v = require(NAT_SYMBOL, y);
    if (v == NULL) {
      v = newNat(asNat(x) * asNat(y));
    }
  }
  return v;
}

static Struct* symbolEq(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(SYMBOL_SYMBOL, x);
  if (v == NULL) {
    v = require(SYMBOL_SYMBOL, y);
    if (v == NULL) {
      if (asSymbol(x) == asSymbol(y)) {
        v = newPrimFun2(kcomb);
      } else {
        v = newPrimFun2(ckcomb);
      }
    }
  }
  return v;
}

static Struct* strEq(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(STR_SYMBOL, x);
  if (v == NULL) {
    v = require(STR_SYMBOL, y);
    if (v == NULL) {
      if (strcmp(asStr(x), asStr(y))) {
        v = newPrimFun2(ckcomb);
      } else {
        v = newPrimFun2(kcomb); // strcmp returns 0 if equal
      }
    }
  }
  return v;
}

static Struct* natEq(Struct* x, Struct* y) {
  Struct* v = NULL;
  v = require(NAT_SYMBOL, x);
  if (v == NULL) {
    v = require(NAT_SYMBOL, y);
    if (v == NULL) {
      if (asNat(x) == asNat(y)) {
        v = newPrimFun2(kcomb);
      } else {
        v = newPrimFun2(ckcomb);
      }
    }
  }
  return v;
}

static Struct* icomb(Struct* x) { return x; }

static Struct* bcomb(Struct* f, Struct* g, Struct* x) {
  Struct* v = apply(g, x);
  if (get_tag(v) != ERROR_SYMBOL) {
    v = apply(f, v);
  }
  return v;
}

static Struct* scomb(Struct* f, Struct* g, Struct* x) {
  Struct* v = apply(f, x);
  if (get_tag(v) != ERROR_SYMBOL) {
    Struct* v2 = apply(g, x);
    if (get_tag(v2) != ERROR_SYMBOL) {
      v = apply(v, v2);
    }
  }
  return v;
}

/* Not a standard combinator AFAIK; x = (c s) */
static Struct* xcomb(Struct* g, Struct* f, Struct* x) {
  Struct* v = apply(f, x);
  if (get_tag(v) != ERROR_SYMBOL) {
    Struct* v2 = apply(g, x);
    if (get_tag(v2) != ERROR_SYMBOL) {
      v = apply(v, v2);
    }
  }
  return v;
}

static Struct* ccomb(Struct* f, Struct* x, Struct* y) {
  Struct* v = apply(f, y);
  if (get_tag(v) != ERROR_SYMBOL) {
    v = apply(v, x);
  }
  return v;
}

static Struct* field(Struct* i, Struct* s) {
  Struct* v = require(STRUCT_SYMBOL, s);
  if (v == NULL) {
    v = require(NAT_SYMBOL, i);
    if (v == NULL) {
      Struct* x = dequote(s);
      size_t n = asNat(i);
      v = (Struct*)get_field(x, n);
      if (v == NULL) {
        v = too_short(get_tag(x), get_size(x), n);
      }
    }
  }
  return v;
}

void printValue(FILE* stream, Struct* e) {
  switch (get_tag(e)) {
  case NAT_SYMBOL     : printNat(stream, e)               ; break;
  case STR_SYMBOL     : printStr(stream, e)               ; break;
  case ERROR_SYMBOL   : printError(stream, e)             ; break;
  case PRIMFUN1_SYMBOL:
  case PRIMFUN2_SYMBOL:
  case CLOHALF_SYMBOL :
  case PRIMFUN3_SYMBOL:
  case CLOTHIRD_SYMBOL:
  case TWOTHIRD_SYMBOL: printPrimFun(stream, e)           ; break;
  case SYMBOL_SYMBOL  : printSymbol(stream, e)            ; break;
  case PAIR_SYMBOL    : printPair(stream, printValue, e)  ; break;
  case STRUCT_SYMBOL  : printStruct(stream, printValue, e); break;
  default             :
    fprintf(stderr, "unknown expr tag %s\n", decompressSymbol(get_tag(e)));
    int UNHANDLED_EXPR_TAG = 0;
    assert(UNHANDLED_EXPR_TAG);
  }
}

Struct* show(Struct* e) {
  if (get_tag(e) == ERROR_SYMBOL) return e;
  char* buffer = NULL;
  size_t sizeloc = 0;
  // https://www.gnu.org/software/libc/manual/html_node/String-Streams.html
  FILE* memStream = open_memstream(&buffer, &sizeloc);
  printValue(memStream, e);
  fclose(memStream);
  // Kludge to marry GC with open_memstream
  // Perhaps better to use a Boehm ropes-based approach in the future
  char* gcBuffer = (char*)GC_MALLOC(sizeloc+1); // +1 is for NUL at end
  strcpy(gcBuffer, buffer);
  free(buffer); buffer = NULL;
  return newStr(gcBuffer);
}

Struct* matchPrim(Symbol name) {
  Struct* p = NULL;
  switch (name) {
  case 1                   /* b           */: p = newPrimFun3(bcomb      ); break;
  case 2                   /* c           */: p = newPrimFun3(ccomb      ); break;
  case 8                   /* i           */: p = newPrimFun1(icomb      ); break;
  case 10                  /* k           */: p = newPrimFun2(kcomb      ); break;
  case 18                  /* s           */: p = newPrimFun3(scomb      ); break;
  case 23                  /* x           */: p = newPrimFun3(xcomb      ); break;
  case 27                  /* +           */: p = newPrimFun2(add        ); break;
  case 29                  /* *           */: p = newPrimFun2(mult       ); break;
  case 322                 /* ck          */: p = newPrimFun2(ckcomb     ); break;
  //case 98449             /* read        */: p =                         ; break;
  //case 361124            /* eval        */: p =                         ; break;
  case 152115              /* true        */: p = newPrimFun2(kcomb      ); break;
  case 735474              /* show        */: p = newPrimFun1(show       ); break;
  case 4795397             /* false       */: p = newPrimFun2(ckcomb     ); break;
  case 4801551             /* parse       */: p = newPrimFun1(parse      ); break;
  case 19543500            /* monus       */: p = newPrimFun2(monus      ); break;
  case 183310355           /* tag-of      */: p = newPrimFun1(tag_of     ); break;
  case 5865881363          /* type-of     */: p = newPrimFun1(type_of    ); break;
  case 32754191373         /* nat-eq?     */: p = newPrimFun2(natEq      ); break;
  case 32754189938         /* str-eq?     */: p = newPrimFun2(strEq      ); break;
  case 3681061522566       /* get-field   */: p = newPrimFun2(field      ); break;
  case 1073289088774930    /* symbol-eq?  */: p = newPrimFun2(symbolEq   ); break;
  case 5392652055496306    /* struct-size */: p = newPrimFun1(struct_size); break;
  case 1152921504606846976 /* _1          */: p = newPrimFun1(fst        ); break;
  case 2305843009213693952 /* _2          */: p = newPrimFun1(snd        ); break;
  case PAIR_SYMBOL                          : p = newPrimFun2(pair       ); break;
  default                                   :                             ; break;
  }
  return p;
}
