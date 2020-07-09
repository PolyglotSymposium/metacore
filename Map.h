#ifndef __MAP_H__
#define __MAP_H__

#include "Symbol.h"
#include "Struct.h"

typedef struct Map Map;
Map* insert(Symbol, Struct*, Map*);
Struct* lookup(Symbol, Map*);

#endif//__MAP_H__
