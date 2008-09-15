#include "Board.h"
#include "Analysis.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    Board board;
    Rect moves1[MAX_MOVES], moves2[MAX_MOVES];
    int values1[MAX_MOVES], values2[MAX_MOVES];
    int num_moves1, num_moves2;
    int n;
    char buf[5];

    analysis_initialize();

    if (argc != 2)
    {
        printf("Usage: analyze-board <board>\n");
        exit(1);
    }

    if ( !board_decode_full(&board, argv[1]) &&
        !board_decode_short(&board, argv[1]) )
    {
        printf("Could not decode board description\n");
        exit(1);
    }

    printf("Board:\n");
    board_print(&board, stdout);

    num_moves1 = analysis_value_moves_normal(&board, moves1, values1);
    num_moves2 = analysis_value_moves_misere(&board, moves2, values2);
    assert(num_moves2 == -1 || num_moves1 == num_moves2);

    printf("Winning moves:\n");
    for (n = 0; n < num_moves1; ++n)
    {
        rect_encode(&moves1[n], buf);
        if (values1[n] <= 0 && values2[n] <= 0) continue;
        printf("%2d. %s %3d %3d\n", n + 1, buf, values1[n], values2[n]);
    }

    return 0;
}

