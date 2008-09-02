#include "Board.h"
#include "Analysis.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "HashTable.h"
extern HashTable *new_cache;

int main()
{
    Board b;
    GroupInfo gi;
    FILE *fp;

    /* TODO: test with more boards */
    board_decode_short(&b, "0V0V0V0V0VVVVVVVVVVV");
    board_print(&b, stdout);
    analysis_initialize();
    analysis_identify_groups(&b, &gi);
    analysis_nim_values(&b, &gi);
    printf("%.3fs\n", (double)clock()/CLOCKS_PER_SEC);
    fp = fopen("memo55.dat", "wb");
    if (fp != NULL)
    {
        int n;
        for (n = 0; n < (1<<25); ++n) fputc(memo[n^((1<<25)-1)], fp);
        fclose(fp);
        printf("memo dumped to memo55.dat\n");
    }
    assert(gi.nval[0] == 8);
    printf("hash table size: %d\n", (int)HT_size(new_cache));
    return 0;
}
