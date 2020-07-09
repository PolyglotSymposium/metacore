#include <assert.h>
#include "SymbolValue.h"
#include "StructValue.h"
#include "Eval.h"
#include "Forms.h"
#include "Error.h"

static Struct* quoteForm(Struct* form) {
  Struct* x = NULL;
  if (get_size(form) == 1) {
    x = (Struct*)get_field(form, 0);
  } else {
    x = malformed(get_tag(form));
  }
  return x;
}

static Struct* constructForm(Struct* form) {
  Struct* x = NULL;
  size_t size = get_size(form);
  if (size > 0) {
    Struct* shouldBeTag = (Struct*)get_field(form, 0);
    if (get_tag(shouldBeTag) == SYMBOL_SYMBOL) {
      Symbol tag = asSymbol(shouldBeTag);
      switch (size) {
      case 1 : x = atomic_struct(tag                         ); break;
      case 2 : x = singleton_struct(tag,   get_field(form, 1)); break;
      default: x = new_struct(tag, size-1, get_fields(form)+1); break;
      }
      x = quote(x);
    }
  }
  if (x == NULL) {
    x = malformed(get_tag(form));
  }
  return x;
}

Struct* matchForm(Struct* form) {
  Struct* x = NULL;
  Symbol tag = get_tag(form);
  switch (tag) {
  case 4831888        /* quote     */: x = quoteForm(form)    ; break;
  case 20981506192834 /* construct */: x = constructForm(form); break;
  default                            : x = noSuchForm(tag)    ; break;
  }
  return x;
}

Map* define(Map* env, Struct* form) {
  assert(get_size(form) == 2);
  Symbol name = (Symbol)get_field(form, 0);
  Struct* value = (Struct*)get_field(form, 1);
  return insert(name, value, env);
}

Map* matchTopLevel(Map* env, Struct* form) {
  Map* m = NULL;
  switch (get_tag(form)) {
  case 148116611 /* define */: m = define(env, form); break;
  default                    : m = NULL             ; break;
  }
  return m;
}
