#ifndef HASHMAP_H_INCLUDED
#define HASHMAP_H_INCLUDED

#include <stdlib.h>
#include <string.h>

typedef struct Entry {
  char *key;
  char *value;
  struct Entry *next;
} Entry;

typedef struct HashMap {
  unsigned int size;
  unsigned int capacity;
  struct Entry **entries;
} HashMap;

HashMap * hashmap_create(const unsigned int capacity);

Entry * hashmap_create_entry();
Entry * hashmap_locate_entry(HashMap *map, const char *key);
Entry * hashmap_entry_for_key(HashMap *map, const char *key);

unsigned int hashmap_put(HashMap *map, const char *key, const char *value);
unsigned int hashmap_get(HashMap *map, const char *key, const char **result);
unsigned int hashmap_remove(HashMap *map, const char *key);
unsigned int hashmap_populate_entry(Entry *entry, const char *key, const char *value);
unsigned int hashmap_replace_entry_value(Entry *entry, const char *value);
unsigned int hashmap_index(HashMap *map, const char *key);
unsigned int hashmap_contains(HashMap *map, const char *key);

unsigned long hashmap_hash(const char *str);

void hashmap_destroy(HashMap *map);
void hashmap_destroy_entry(Entry *entry);
void hashmap_clear(HashMap *map);
void hashmap_clear_index(HashMap *map, const unsigned int index);

#endif
