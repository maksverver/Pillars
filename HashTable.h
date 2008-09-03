#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Hash table data structure with constant-sized keys and a fixed index size.
   All keys must be distinct; key and value pointers may not be NULL.
   Memory for the keys and values must be managed by the caller.
*/

typedef struct HashTable HashTable;

/* Create a new hash table. */
HashTable *HT_create(size_t key_size, size_t index_size,
                     uint32_t (*hash_func)(const void *data, size_t len) );

/* Destroy a hash table. */
void HT_destroy(HashTable *ht);

/* Returns the number of entries stored in the hash table. */
size_t HT_size(HashTable const *ht);

/* Update an entry.
   Returns wether an existing entry was overwritten; if so, *old_key and
   *old_value are set to the old key and value pointers. */
bool HT_set(HashTable *ht,
            void const *key, void const *value,
            void **old_key, void **old_value);

/* Retrieve a value by key.
   Returns the corresponding value, or NULL if none is found. */
void *HT_get(HashTable const *ht, void const *key);

/* Retrieve a complete entry by key.
   Returns wether the entry was found; if so, *old_key and *old_value
   are set to the key and value pointers of the stored entry. */
bool HT_get_entry(HashTable const *ht,
                  void const *key,
                  void **old_key,
                  void **old_value);

/* Retrieve a value by key, if it does not exist, or insert it otherwise.
   Returns the existing value, or NULL if none is found. */
void *HT_get_or_set(HashTable *ht, void const *key, void const *value);

/* Remove an entry with the given key.
   Returns whether the entry was found; if so, *old_key and *old_value
   are set to the old key and value pointers. */
bool HT_remove(HashTable *ht,
               void const *key,
               void **old_key,
               void **old_value);

#endif /* ndef HASH_TABLE_H_INCLUDED */
