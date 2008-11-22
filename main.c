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

    if (strcmp(buf, "Quit") == 0) return NULL;

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

/* Determine a tiebreaker value for the given move. (Higher is better)
   Current tie-breaker: size of largest group left after making the move
   (motivation: this makes it hard for the opponent to compute the exact result
    and is more likely to cause him to make a wrong move) */
int tiebreaker(Board *brd, Rect *move)
{
    Board tmp;
    GroupInfo gi;
    int n, max_size;

    memcpy(&tmp, brd, sizeof(Board));
    board_fill(&tmp, move, -1);
    analysis_identify_groups(&tmp, &gi);

    max_size = 0;
    for (n = 0; n < gi.num_groups; ++n)
    {
        if (gi.size[n] > max_size) max_size = gi.size[n];
    }
    return max_size;
}

void select_move(Board *brd, Rect *move, bool *use_joker)
{
    Rect moves[MAX_MOVES];
    int values[MAX_MOVES];
    int num_moves, n, cnt[7], best_val, tb, best_tb;
    char buf[26];
    bool fallback;

    board_encode_short(brd, buf);
    info("Analyzing board: %s", buf);
    num_moves = analysis_value_moves_misere(brd, moves, values);
    if (num_moves >= 0)
    {
        info("Misere play");
    }
    else
    {
        num_moves = analysis_value_moves_normal(brd, moves, values);
        info("Normal play");
    }
    if (num_moves == 0)
    {
        fatal("No valid moves available");
    }

    fallback = false;
search_values:
    best_val = -3;
    memset(cnt, 0, sizeof(cnt));
    for (n = 0; n < num_moves; ++n)
    {
        if (values[n] > best_val) best_val = values[n];
        assert(values[n] >= -3 && values[n] <= +3);
        ++cnt[values[n] + 3];
    }
    if (best_val == -3)
    {
        info("All misere moves are losing; fall back to normal play");
        fallback = true;
        num_moves = analysis_value_moves_normal(brd, moves, values);
        goto search_values;
    }

    *use_joker = !fallback && (best_val >= +2);

    shuffle_moves_and_values(moves, values, num_moves);
    best_tb = -1;
    for (n = 0; n < num_moves; ++n)
    {
        if (values[n] == best_val)
        {
            tb = tiebreaker(brd, &moves[n]);
            if (tb > best_tb)
            {
                *move = moves[n];
                best_tb = tb;
            }
        }
    }

    info( "-3/-2/-1/0/+1/+2/+3: %d/%d/%d/%d/%d/%d/%d (best: %d; tiebreaker: %d)",
          cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5], cnt[6], best_val, best_tb );
}

int main(int argc, char *argv[])
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

    if (argc < 1 || argc > 2)
    {
        fprintf(stderr, "Usage: player [<board>]\n");
        exit(1);
    }

    if (argc == 2)
    {
        if (!board_decode(&board, argv[1]))
        {
            fprintf(stderr, "Invalid board description: %s", argv[1]);
            exit(1);
        }
    }
    else
    {
        /* Initialize board from pillars */
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
    }

    info("Initialization complete.");
    total_time = time_now();

    turn = 0;
    joker = true;
    for (;;)
    {
        Rect move;
        char buf[64];
        double t0, t1;

        t0 = time_now();
        if ((line = next_line()) == NULL) break;
        t1 = time_now();
        info("Received %s (waited %.3fs)", line, t1 - t0);
        t0 = t1;

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
        t1 = time_now();
        total_time += t1 - t0;
        info("Sending %s (calculated for %.3fs; %.3fs total)", buf, t1 - t0, total_time);
        fprintf(stdout, "%s\n", buf);
        fflush(stdout);
    }
    info("Exiting (used %.3fs)", total_time);

    return 0;
}
