#include "Debug.h"
#include "Board.h"
#include "Analysis.h"
#include <string.h>
#include <stdlib.h>

static const char *next_line()
{
    static char buf[1024];
    size_t len;

    if (!fgets(buf, sizeof(buf), stdin)) return NULL;

    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n')
    {
        buf[len - 1] = '\0';
        --len;
    }

    return buf;
}

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

    fatal("No valid moves available");
}

int main()
{
    Board board;
    const char *line;
    int r, c, n;
    int turn;

    time_reset();
    analysis_initialize();

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

        line = next_line();
        if (!point_decode(&p, line))
        {
            fatal("Invalid point received: %s", line);
        }
        board[p.r][p.c] = -1;
    }

    turn = 0;
    while((line = next_line()) != NULL)
    {
        Rect move;
        char buf[64];

        info("Received: %s", line);
        if (strcmp(line, "Start") == 0) goto start;
        if (!rect_decode(&move, line) || !board_is_valid_move(&board, &move))
        {
            fatal("Invalid move received: %s", buf);
        }
        board_fill(&board, &move, ++turn);
        if (board_empty_area(&board) == 0)
        {
            info("Board is full");
            break;
        }
    start:
        select_move(&board, &move);
        board_fill(&board, &move, ++turn);
        rect_encode(&move, buf);
        info("Sending: %s", buf);
        fprintf(stdout, "%s\n", buf);
        fflush(stdout);
    }
    info("Exiting");

    return 0;
}
