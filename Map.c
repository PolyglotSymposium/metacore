#include "Map.h"
#include <assert.h>
#include <gc.h>

struct Map {
  Map* before;
  Symbol key;
  Struct* value;
  Map* after;
};

Map* insert(Symbol key, Struct* value, Map* map) {
  Map* m;
  if (map == NULL) {
    m = (Map*)GC_MALLOC(sizeof(Map));
    m->before = NULL;
    m->key = key;
    m->value = value;
    m->after = NULL;
  } else if (key < map->key) {
    m = (Map*)GC_MALLOC(sizeof(Map));
    m->before = insert(key, value, map->before);
    m->key = map->key;
    m->value = map->value;
    m->after = map->after;
  } else if (key > map->key) {
    m = (Map*)GC_MALLOC(sizeof(Map));
    m->before = map->before;
    m->key = map->key;
    m->value = map->value;
    m->after = insert(key, value, map->after);
  } else {
    assert(key != map->key);
  }
  return m;
}

Struct* lookup(Symbol key, Map* map) {
  Struct* value;
  if (map == NULL) {
    value = NULL;
  } else if (key < map->key) {
    value = lookup(key, map->before);
  } else if (key > map->key) {
    value = lookup(key, map->after);
  } else {
    value = map->value;
  }
  return value;
}
