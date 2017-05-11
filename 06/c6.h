#ifndef __C6__
#define __C6__

#include <string.h>

typedef char* string;

#define c6size(array) (sizeof(array)/sizeof(array[0]))

typedef struct _Pair {
  char *key;
  void *value;
} Pair;

typedef struct _Map {
  Pair *list;
  int size;
  int top;
} Map;

extern Map* c6new(Map *map, Pair *list, int size);
extern Pair c6add(Map *map, char *key, void *value);
extern int c6find(Map *map, char *key);
extern void* c6lookup(Map *map, char *key);
extern char *c6strnew(char *str);
extern void c6htob(string hex, string binary);
extern void c6dtob(int d, string binary);

#endif
