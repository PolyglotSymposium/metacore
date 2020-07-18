#include "Primitives.h"
#include "Eval.h"
#include "StructParser.h"
#include <assert.h>
#include <string.h>

int main(int argc, char* argv[])
{
  Map* env = NULL;
  char* code = NULL;
  int cont = true;
  if (argc > 1) {
    code = argv[1];
    Struct* e = chars_to_struct(code);
    if (e == NULL) {
      fprintf(stderr, "Parse error\n");
    } else {
      Map* envOut = NULL;
      e = evalTopLevel(env, &envOut, e);
      if (e != NULL) { printValue(stdout, e); printf("\n"); }
    }
  } else {
    while (cont) {
      size_t len = 0;
      getline(&code, &len, stdin); assert(code != NULL);
      if (strcmp(",q\n", code)) {
        Struct* e = chars_to_struct(code);
        code = NULL;
        if (e == NULL) {
          fprintf(stderr, "Parse error\n");
        } else {
          Map* envOut = NULL;
          e = evalTopLevel(env, &envOut, e);
          if (e != NULL) { printValue(stdout, e); printf("\n"); }
          env = envOut;
        }
      } else {
        cont = false;
      }
    }
  }
  return 0;
}
