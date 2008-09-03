#include "Board.h"
#include "Analysis.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "HashTable.h"
extern HashTable *new_cache;

typedef struct Entry
{
    struct Entry *next;
    void *key;
    void *value;
} Entry;

struct HashTable
{
    size_t key_size, index_size, size;
    uint32_t (*hash_func)(void const *data, size_t len);
    Entry **index;
};

void print_hashtable_info(HashTable *ht)
{
    int cnt[11] = { }, n;
    printf("Hash table info:\n");
    printf("Index size: %12d\n", (int)ht->index_size) ;
    printf("Population: %12d\n", (int)ht->size);
    for (n = 0; n < (int)ht->index_size; ++n)
    {
        Entry *e;
        int c = 0;
        for (e = (ht->index)[n]; e != NULL; e = e->next)
        {
            if (++c == 10) break;
        }
        cnt[c] += 1;
    }
    printf("Bucket population:\n");
    for(n = 0; n < 10; ++n) printf("%2d: %12d\n", n, cnt[n]);
}

int main()
{
    Board b;
    GroupInfo gi;
    /* FILE *fp; */

    /* TODO: test with more boards */
    board_decode_short(&b, "0V0V0V0V0VVVVVVVVVVV");
    board_print(&b, stdout);
    analysis_initialize();
    analysis_identify_groups(&b, &gi);
    analysis_nim_values(&b, &gi);
    printf("%.3fs\n", (double)clock()/CLOCKS_PER_SEC);
    /*
    fp = fopen("memo55.dat", "wb");
    if (fp != NULL)
    {
        int n;
        for (n = 0; n < (1<<25); ++n) fputc(memo[n^((1<<25)-1)], fp);
        fclose(fp);
        printf("memo dumped to memo55.dat\n");
    }
    */
    assert(gi.nval[0] == 8);
    print_hashtable_info(new_cache);
    return 0;
}
