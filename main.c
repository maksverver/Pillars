#include "Board.h"
#include <assert.h>

/* Brain-dead test player */
void select_move(Board *brd, Rect *move)
{
    for (move->p.r = 0; move->p.r < 10; ++move->p.r)
    {
        for (move->p.c = 0; move->p.c < 10; ++move->p.c)
        {
            if ((*brd)[move->p.r][move->p.c] != 0) continue;

            move->q.r = move->p.r + 1;
            move->q.c = move->p.c + 1;
            while (move->q.r < 10)
            {
                ++move->q.r;
                if (!board_is_valid_move(brd, move))
                {
                    move->q.r--;
                    break;
                }
            }
            while (move->q.c < 10)
            {
                ++move->q.c;
                if (!board_is_valid_move(brd, move))
                {
                    move->q.c--;
                    break;
                }
            }
            return;
        }
    }

    fprintf(stderr, "No valid moves available!\n");
    abort();
}

int main()
{
    Board board;
    char buf[64];
    int r, c, n;
    int turn;

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            board[r][c] = 0;
        }
    }

    for (n = 0; n < 10; ++n)
    {
        Point p;
        fgets(buf, sizeof(buf), stdin);
        if (point_decode(&p, buf))
        {
            board[p.r][p.c] = -1;
        }
        else
        {
            fprintf(stderr, "Invalid point received: %s\n", buf);
            abort();
        }
    }

    turn = 0;
    while(fgets(buf, sizeof(buf), stdin) != NULL)
    {
        Rect move;

        if (strcmp(buf, "Start\n") == 0) goto start;
        if (!rect_decode(&move, buf) || !board_is_valid_move(&board, &move))
        {
            fprintf(stderr, "Invalid move received: %s\n", buf);
            abort();
        }
        board_fill(&board, &move, ++turn);
        if (board_empty_area(&board) == 0)
        {
            fprintf(stderr, "Board is full.\n");
            break;
        }
    start:
        select_move(&board, &move);
        board_fill(&board, &move, ++turn);
        rect_encode(&move, buf);
        fprintf(stdout, "%s\n", buf);
        fflush(stdout);
    }

    fprintf(stderr, "Exiting.\n");

    return 0;
}
