#include "Primitives.h"
#include "Eval.h"
#include "StructParser.h"
#include <assert.h>

int main(int argc, char* argv[])
{
  Map* env = NULL;
  char* code = NULL;
  while (true) {
    size_t len = 0;
    getline(&code, &len, stdin); assert(code != NULL);
    Struct* e = chars_to_struct(code);
    code = NULL;
    if (e == NULL) {
      return 1;
    } else {
      Map* envOut = NULL;
      e = evalTopLevel(env, envOut, e);
      if (e != NULL) { printValue(stdout, e); }
      env = envOut;
      return 0;
    }
  }
}
