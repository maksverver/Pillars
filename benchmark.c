#include "Board.h"
#include "Analysis.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>

int main()
{
    Board b;
    GroupInfo gi;

    /* TODO: test with more boards */
    board_decode_short(&b, "0V0V0V0V0VVVVVVVVVVV");
    board_print(&b, stdout);
    analysis_initialize();
    analysis_identify_groups(&b, &gi);
    analysis_nim_values(&b, &gi);
    assert(gi.nval[0] == 8);
    printf("%.3fs\n", (double)clock()/CLOCKS_PER_SEC);
    return 0;
}
