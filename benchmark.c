#include "Board.h"
#include "Analysis.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

/*
const char *board_desc = "7VFVVVVVVVVVVVVVVVVV";
int nval = 3;
*/

/*
const char *board_desc = "0V0V0V0VVVVVVVVVVVVV";
int nval = 9;
*/

/*
const char *board_desc = "0V0V0V0V0VVVVVVVVVVV";
int nval = 8;
*/

const char *board_desc = "OV0180V3VVVVVVVVVVVV";
int nval = 24;

void debug_cache_info();

int main()
{
    Board b;
    GroupInfo gi;
    /* FILE *fp; */

    /* TODO: test with more boards */
    board_decode_short(&b, board_desc);
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
    assert(gi.nval[0] == nval);
    debug_cache_info();
    return 0;
}
