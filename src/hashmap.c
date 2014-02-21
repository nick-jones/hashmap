#include "hashmap.h"

/*
 * Create a new hashmap. The number of entries (slots) will be determined by
 * the supplied capacity argument. Note that currently the capacity is fixed,
 * i.e. no resize is implemented.
 */
HashMap * hashmap_create(const unsigned int capacity) {
  // Allocate memory for the map.
  HashMap *map = malloc(sizeof(HashMap));

  if (map == NULL) {
    return NULL;
  }

  // Allocate memory for the entries. HashMap::entries is Entry**, so memory
  // for capacity × Entry* must be allocated.
  map->entries = malloc(sizeof(Entry *) * capacity);

  // Handle allocation failure. Since map has been malloc'd at this stage, it
  // must be freed prior to return.
  if (map->entries == NULL) {
    free(map);
    return NULL;
  }

  // Initialise the struct variables.
  memset(map->entries, 0, sizeof(Entry *) * capacity);
  map->size = 0;
  map->capacity = capacity;

  return map;
}

/*
 * Destroys a HashMap. All entries will be removed, and memory will be freed.
 */
void hashmap_destroy(HashMap *map) {
  hashmap_clear(map);

  free(map->entries);
  free(map);
}

/*
 * Retrieve the value associated with the supplied key. Returned integer
 * indicates whether the fetch was successful. If successful, the supplied
 * result argument will contain a pointer to the value. Note that this can
 * potentially dangle, since the associated entry can be removed at any
 * point.. it's advisable to copy the value out after fetch.
 */
unsigned int hashmap_get(HashMap *map, const char *key, const char **result) {
  Entry *entry = hashmap_locate_entry(map, key);
  
  if (entry == NULL) {
    return 0;
  }

  *result = entry->value;

  return 1;
}

/*
 * Indicates whether the hashmap contains an entry with some key.
 */
unsigned int hashmap_contains(HashMap *map, const char *key) {
  Entry *entry = hashmap_locate_entry(map, key);

  return entry == NULL ? 0 : 1;
}

/*
 * A function that is useful internally, this locates an entry by key.
 */
Entry * hashmap_locate_entry(HashMap *map, const char *key) {
  unsigned int index = hashmap_index(map, key);
  Entry *entry;

  for (entry = map->entries[index]; entry != NULL; entry = entry->next) {
    // Compare each entries key against the supplied key.
    if (strcmp(entry->key, key) == 0) {
      return entry;
    }
  }

  return NULL;
}

/*
 * Put a value associated with the supplied key. If an entry already exists
 * with that key, the value will be replaced. Otherwise, a new entry will
 * be created.
 */
unsigned int hashmap_put(HashMap *map, const char *key, const char *value) {
  Entry *entry = hashmap_entry_for_key(map, key);

  if (entry == NULL) {
    return 0;
  }

  // Entry already exists, since a key is present.
  if (entry->key) {
    // Simply replace the value.
    return hashmap_replace_entry_value(entry, value);
  }

  // Previously unseen entry.
  map->size++;
  
  return hashmap_populate_entry(entry, key, value);
}

/*
 * Retrieves or creates an entry for a given key. The entry will always be
 * added into the map, either as the root of a slot, or the tail of the
 * linked-list within a slot.
 */
Entry * hashmap_entry_for_key(HashMap *map, const char *key) {
  unsigned int index = hashmap_index(map, key);
  Entry *existing = map->entries[index];
  Entry *previous = NULL;
  
  // Loop through all possible entries in the linked-list.
  while (existing != NULL) {
    // If the supplied key matches that of the entry, then return it.
    if (strcmp(existing->key, key) == 0) {
      return existing;
    }

    previous = existing;
    existing = existing->next;
  }

  // No previous entries with the supplied key, so create a new one.
  Entry *entry = hashmap_create_entry();

  // Allocation failure, bail.
  if (entry == NULL) {
    return NULL;
  }

  if (previous == NULL) { 
    // No root, this entry will consume the slot
    map->entries[index] = entry;
  }
  else {
    // Nodes exist in this slot, add the new entry to the tail
    previous->next = entry;
  }
  
  return entry;
}

/*
 * Creates a new Entry instance. Memory is allocated and variables are
 * initialised appropriately.
 */
Entry * hashmap_create_entry() {
  // Allocate memory for the entry.
  Entry *entry = malloc(sizeof(Entry));

  // Handle allocation failure.
  if (entry == NULL) {
    return NULL;
  }

  // Initialise the struct variables.
  entry->key = NULL;
  entry->value = NULL;
  entry->next = NULL;

  return entry;
}

/*
 * Destroy a map entry. This simply frees allocated memory, it does not
 * remove it from any map. See clear/remove for such functionality.
 */
void hashmap_destroy_entry(Entry *entry) {
  free(entry->key);
  free(entry->value);
  free(entry);
}

/*
 * Indicates which slot a given key must reside in.
 */
unsigned int hashmap_index(HashMap *map, const char *key) {
  // Calculate the hash for the key.
  unsigned long hash = hashmap_hash(key);

  return hash % map->capacity;
}

/*
 * Hashes the supplied string into a long. This operates on the characters of
 * the string.
 *
 * Implementation of djb2, adapted from: http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long hashmap_hash(const char *str) {
  unsigned long hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }

  return hash;
}

/*
 * Replace the value for a given entry. The value is reallocated to be able
 * to consume the new value.
 */
unsigned int hashmap_replace_entry_value(Entry *entry, const char *value) {
  // The +1 accounts for the NULL in the char array
  char *new_value = realloc(entry->value, (strlen(value) + 1) * sizeof(char));

  // Handle allocation failure.
  if (new_value == NULL) {
    return 0;
  }  

  // Copy the new value into the entry.
  entry->value = new_value;
  strcpy(entry->value, value);

  return 1;
}

/*
 * Populate an entry with the supplied key and value. This assumes that
 * the entry is fresh - do not use this for already populated entries.
 */
unsigned int hashmap_populate_entry(Entry *entry, const char *key,
                                    const char *value) {
  // Allocate memory for the key, +1 accounting for the NULL
  entry->key = malloc((strlen(key) + 1) * sizeof(char));
  
  if (!entry->key) {
    return 0;
  }

  // Allocate memory for the value, +1 for the NULL
  entry->value = malloc((strlen(value) + 1) * sizeof(char));

  if (!entry->value) {
    free(entry->key);
    return 0;
  }

  // Copy the key and value into the struct variables.
  strcpy(entry->key, key);
  strcpy(entry->value, value);

  return 1;
}

/*
 * Remove an entry based on key.
 */
unsigned int hashmap_remove(HashMap *map, const char *key) {
  unsigned int index = hashmap_index(map, key);
  unsigned int found = 0;
  Entry *previous = NULL;
  Entry *entry;

  // Locate the entry in the linked-list with key comparison.
  for (entry = map->entries[index]; entry != NULL; entry = entry->next) {
    if (strcmp(entry->key, key) == 0) {
      found = 1;
      break;
    }

    previous = entry;
  }

  // Not hits for the supplied key, indicate failure.
  if (found == 0) {
    return 0;
  }

  if (previous) {
    // If we have a previous node, we simply need to jump over the current
    // entry.
    previous->next = entry->next;
  }
  else {
    // Otherwise we're dealing with the root. The next node should take over
    // the slot (if it is NULL, this is not a problem.)
    map->entries[index] = entry->next;
  }

  hashmap_destroy_entry(entry);
  map->size--;

  return 1;
}

/*
 * Clears a HashMap of all entries.
 */
void hashmap_clear(HashMap *map) {
  unsigned int i;

  for (i = 0; i < map->capacity; i++) {
    hashmap_clear_index(map, i);
  }
}

/*
 * Clears all entries for a given index.
 */
void hashmap_clear_index(HashMap *map, const unsigned int index) {
  Entry *entry = map->entries[index];
  Entry *next;

  while (entry != NULL) {
    next = entry->next;
    entry->next = NULL;

    hashmap_destroy_entry(entry);
    map->size--;
    
    entry = next;
  }

  map->entries[index] = NULL;
}
