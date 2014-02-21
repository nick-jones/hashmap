#include "hashmap_debug.h"

void hashmap_dump(HashMap *map) {
  unsigned int i;

  for (i = 0; i < map->capacity; i++) {
    printf("Slot: %d\n", i);
    Entry *entry = map->entries[i];

    if (entry == NULL) {
      printf(" Empty\n");
      continue;
    }

    do {
      printf(" * key: %s, value: %s\n", entry->key, entry->value);
      entry = entry->next;
    }
    while (entry != NULL);
  }
}
