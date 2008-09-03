#include "HashTable.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct Entry
{
    struct Entry *next;
    uint32_t key_hash;
    void *key;
    void *value;
} Entry;

struct HashTable
{
    size_t key_size, index_size, size;
    uint32_t (*hash_func)(void const *data, size_t len);
    Entry **index;
};

static uint32_t fnv1(void const *data, size_t len)
{
    uint32_t res = 0x01000193u;
    const uint8_t *begin = data, *end = begin + len;
    while (begin != end)
    {
        res += (res<<1) + (res<<4) + (res<<7) + (res<<8) + (res<<24);
        res ^= *begin++;
    }
    return res;
}

static Entry **find(HashTable const *ht, uint32_t h, void const *key)
{
    Entry **e;
    for (e = ht->index + h%ht->index_size; *e != NULL; e = &(*e)->next)
    {
        if ((*e)->key_hash == h && memcmp((*e)->key, key, ht->key_size) == 0) break;
    }
    return e;
}

HashTable *HT_create(size_t key_size, size_t index_size,
                     uint32_t (*hash_func)(void const *data, size_t len))
{
    HashTable *ht;

    ht = malloc(sizeof(HashTable) + sizeof(Entry*)*index_size);
    assert(ht != NULL);
    ht->key_size   = key_size;
    ht->index_size = index_size;
    ht->hash_func  = hash_func == NULL ? &fnv1 : hash_func;
    ht->size       = 0;
    ht->index      = (void*)((char*)ht + sizeof(HashTable));

    return ht;
}

void HT_destroy(HashTable *ht)
{
    Entry *e, *f;
    size_t i;

    for (i = 0; i < ht->index_size; ++i)
    {
        for (e = ht->index[i]; e != NULL; e = f)
        {
            f = e->next;
            free(e);
        }
    }
    free(ht);
}

size_t HT_size(HashTable const *ht)
{
    return ht->size;
}


bool HT_set(HashTable *ht,
            void const *key, void const *value,
            void **old_key, void **old_value)
{
    uint32_t h = (*ht->hash_func)(key, ht->key_size);
    Entry **e = find(ht, h, key);

    if (*e != NULL)
    {
        if (old_key)   *old_key   = (*e)->key;
        if (old_value) *old_value = (*e)->value;

        (*e)->key_hash  = h;
        (*e)->key       = (void*)key;
        (*e)->value     = (void*)value;

        return true;
    }
    else
    {
        *e = malloc(sizeof(Entry));
        assert(*e != NULL);

        (*e)->key_hash  = h;
        (*e)->key       = (void*)key;
        (*e)->value     = (void*)value;

        ht->size += 1;
        return false;
    }
}

void *HT_get(HashTable const *ht, void const *key)
{
    uint32_t h = (*ht->hash_func)(key, ht->key_size);
    Entry **e = find(ht, h, key);
    if (*e == NULL) return NULL;
    return (*e)->value;
}

bool HT_get_entry(HashTable const *ht,
                  void const *key,
                  void **old_key,
                  void **old_value)
{
    uint32_t h = (*ht->hash_func)(key, ht->key_size);
    Entry **e = find(ht, h, key);
    if (*e == NULL) return false;
    if (old_key)   *old_key   = (*e)->key;
    if (old_value) *old_value = (*e)->value;
    return true;
}

/* Retrieve a value by key, if it does not exist, or insert it otherwise.
   Returns the existing value, or NULL if none is found. */
void *HT_get_or_set(HashTable *ht, void const *key, const void *value)
{
    uint32_t h = (*ht->hash_func)(key, ht->key_size);
    Entry **e = find(ht, h, key);
    if (*e != NULL) return (*e)->value;
    *e = malloc(sizeof(Entry));
    assert(*e != NULL);
    (*e)->key_hash = h;
    (*e)->key      = (void*)key;
    (*e)->value    = (void*)value;
    ht->size += 1;
    return NULL;
}

bool HT_remove(HashTable *ht,
               void const *key,
               void **old_key,
               void **old_value)
{
    uint32_t h = (*ht->hash_func)(key, ht->key_size);
    Entry *f, **e = find(ht, h, key);
    if (*e == NULL) return false;

    /* This function is actually untested! */
    assert(0);

    f = *e;
    *e = f->next;
    if (*old_key)   *old_key   = f->key;
    if (*old_value) *old_value = f->value;
    free(f);

    ht->size -= 1;
    return true;
}
