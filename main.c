#include "Debug.h"
#include "Board.h"
#include "Analysis.h"
#include <string.h>
#include <stdlib.h>

#define TIME_LIMIT 5

/* For seeding the RNG: */
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

static void seed_rng(unsigned seed)
{
    if (seed == (unsigned)-1)
    {
        seed = (unsigned)time(NULL) ^ (unsigned)getpid();
    }
    info("Seeding RNG with %u", seed);
    srand(seed);
}

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

static void shuffle_moves_and_values(Rect *moves, int *values, int num_moves)
{
    Rect r;
    int  j, i, v;

    j = num_moves;
    while (j > 0)
    {
        i = rand()%j--;

        /* Swap moves[i] and moves[j] */
        r = moves[i];
        moves[i] = moves[j];
        moves[j] = r;

        /* Swap values[i] and values[j] */
        v = values[i];
        values[i] = values[j];
        values[j] = v;
    }
}

void select_move(Board *brd, Rect *move, bool *use_joker)
{
    Rect moves[MAX_MOVES];
    int values[MAX_MOVES];
    int num_moves, n, cnt[5], best_val;

    info("Analyzing board:");
    board_print(brd, stderr);
    num_moves = analysis_value_moves(brd, moves, values);
    info("%d moves found.", num_moves);
    if (num_moves == 0)
    {
        fatal("No valid moves available");
    }

    best_val = -2;
    memset(cnt, 0, sizeof(cnt));
    for (n = 0; n < num_moves; ++n)
    {
        if (values[n] > best_val) best_val = values[n];
        assert(values[n] >= -2 && values[n] <= +2);
        ++cnt[values[n] + 2];
    }
    assert(cnt[2] == 0);
    info("-2/-1/+1/+2: %d/%d/%d/%d (best value: %d)",
         cnt[0], cnt[1], cnt[3], cnt[4], best_val);

    *use_joker = (best_val == +2);

    shuffle_moves_and_values(moves, values, num_moves);
    for (n = 0; n < num_moves; ++n)
    {
        if (values[n] == best_val)
        {
            *move = moves[n];
            return;
        }
    }
    fatal("shouldn't get here");
}

int main()
{
    Board board;
    const char *line;
    int r, c, n;
    int turn;
    double total_time;
    bool joker, use_joker;

    time_reset();
    analysis_initialize();
    seed_rng((unsigned)-1);

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

    info("Initialization complete.");
    total_time = time_now();

    turn = 0;
    joker = true;
    while((line = next_line()) != NULL)
    {
        Rect move;
        char buf[64];

        time_reset();
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
        select_move(&board, &move, &use_joker);
        if (joker && use_joker)
        {
            joker = false;
            buf[0] = '!';
            rect_encode(&move, buf + 1);
        }
        else
        {
            rect_encode(&move, buf);
        }
        board_fill(&board, &move, ++turn);
        total_time += time_now();
        info("Time left: %.3fs\n", TIME_LIMIT - total_time);
        info("Sending: %s", buf);
        fprintf(stdout, "%s\n", buf);
        fflush(stdout);
    }
    info("Exiting (time used: %.3fs)", total_time);

    return 0;
}
