#include "HashTable.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct Entry
{
    struct Entry *next;
    void *key;
    void *value;
} Entry;

struct HashTable
{
    size_t key_size, index_size, size;
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

static Entry **find(HashTable const *ht, void const *key)
{
    Entry **e = ht->index + fnv1(key, ht->key_size)%ht->index_size;
    while (*e != NULL && memcmp((*e)->key, key, ht->key_size) != 0) e = &(*e)->next;
    return e;
}

HashTable *HT_create(size_t key_size, size_t index_size)
{
    HashTable *ht;

    ht = malloc(sizeof(HashTable) + sizeof(Entry*)*index_size);
    assert(ht != NULL);
    ht->key_size   = key_size;
    ht->index_size = index_size;
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

bool HT_set(HashTable *ht,
            void const *key, void const *value,
            void **old_key, void **old_value)
{
    Entry **e = find(ht, key);
    if (*e != NULL)
    {
        if (old_key)   *old_key   = (*e)->key;
        if (old_value) *old_value = (*e)->value;
        (*e)->key   = (void*)key;
        (*e)->value = (void*)value;
        return true;
    }
    else
    {
        *e = malloc(sizeof(Entry));
        assert(*e != NULL);
        (*e)->key   = (void*)key;
        (*e)->value = (void*)value;
        ht->size += 1;
        return false;
    }
}

void *HT_get(HashTable const *ht, void const *key)
{
    return *find(ht, key);
}

bool HT_get_entry(HashTable const *ht,
                  void const *key,
                  void **old_key,
                  void **old_value)
{
    Entry **e = find(ht, key);
    if (*e == NULL) return false;
    if (old_key)   *old_key   = (*e)->key;
    if (old_value) *old_value = (*e)->value;
    return true;
}

bool HT_remove(HashTable *ht,
               void const *key,
               void **old_key,
               void **old_value)
{
    Entry *f, **e = find(ht, key);
    if (*e == NULL) return false;

    f = *e;
    *e = f->next;
    if (*old_key)   *old_key   = f->key;
    if (*old_value) *old_value = f->value;
    free(f);

    ht->size -= 1;
    return true;
}
