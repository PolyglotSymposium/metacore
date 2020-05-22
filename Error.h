#ifndef __METACORE_ERROR_H__
#define __METACORE_ERROR_H__

#include <stdio.h>
#include "Struct.h"

Struct* typeError(Symbol required, Symbol actual);
Struct* undefined(Symbol);
Struct* noSuchForm(Symbol);
Struct* inapplicable(Symbol);
const Symbol ERROR_SYMBOL = 18302500; /* error */
Symbol asErrorCode(Struct*);
void printError(FILE*, Struct*);

#endif
