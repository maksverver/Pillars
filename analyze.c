#include "Board.h"
#include "Analysis.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void print_cdata(const char *msg)
{
    while (*msg)
    {
        int i = *(unsigned char*)msg++;
        if (i < 32 || i >= 127)
        {
            printf("&#%d;", i);
        }
        else
        {
            switch (i)
            {
            case '<':  printf("&lt;"); break;
            case '>':  printf("&gt;"); break;
            case '&':  printf("&amp;"); break;
            case '"':  printf("&quot;"); break;
            case '\'': printf("&apos;"); break;
            default:   putchar(i);
            }
        }
    }
}

static void print_pillars(Board *b)
{
    int r, c;

    /* TODO: determine permutation */

    printf("<pillars>\n");
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] < 0)
            {
                (*b)[r][c] = -1;
                printf("<point>%c%c</point>", "ABCDEFGHIJ"[r], "abcdefghij"[c]);
            }
        }
    }
    printf("</pillars>\n");
}

static void print_moves(Board *b)
{
    int num_moves, n, r, c;
    Rect move;

    num_moves = 0;
    while (board_get_move(b, NULL, num_moves)) ++num_moves;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] > num_moves) (*b)[r][c] = 0;
        }
    }

    printf("<moves count='%d'>\n", num_moves);
    for (n = 0; n < num_moves; ++n)
    {
        board_get_move(b, &move, n);
        printf("<rect>%c%c%c%c</rect>\n",
            "ABCDEFGHIJ"[move.p.r], "abcdefghij"[move.p.c],
            "ABCDEFGHIJ"[move.q.r], "abcdefghij"[move.q.c]);
    }
    printf("</moves>\n");
}

static void print_board(Board *b)
{
    char buf[201];

    board_encode_full(b, buf);
    printf("<board>%s</board>\n", buf);
}

static void print_message(Board *b)
{
    /* TODO: figure out if the game is finished
             (and if so, winner/score1/score2) */
    printf("<result>\n");
    printf("</result>\n");
}

int main(int argc, char *argv[])
{
    Board b;
    const char *desc, *player1, *player2, *result;

    analysis_initialize();

    if (argc != 2 && argc != 4)
    {
        fprintf(stderr, "Usage: analyze <board> [<player1> <player2>]\n");
        fprintf(stderr, "       analyze <game-file>\n");
        exit(1);
    }
    desc    = argc > 1 ? argv[1] : NULL;
    player1 = argc > 2 ? argv[2] : "Red";
    player2 = argc > 3 ? argv[3] : "Blue";
    result  = argc > 4 ? argv[4] : NULL;

    if (!board_decode_full(&b, argv[1]) && !board_decode_short(&b, argv[1]))
    {
        fprintf(stderr, "Unable to decode board definition!\n");
        exit(1);
    }

    printf("<?xml version='1.0' encoding='UTF-8'?>\n");
    printf("<game>\n");

    if (player1 != NULL)
    {
        printf("<player1><name>");
        print_cdata(player1);
        printf("</name></player1>\n");
    }

    if (player2 != NULL)
    {
        printf("<player2><name>");
        print_cdata(argv[3]);
        printf("</name></player2>\n");
    }

    print_pillars(&b);
    print_moves(&b);
    print_board(&b);
    print_result(&b);
    print_analysis(&b);

    printf("</game>\n");

    return 0;
}
