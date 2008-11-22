#include "Analysis.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool analyis_initialized = false;    /* has the module been initialized? */


/* Determine the nim value for the given group. */
static NV nvalue(Board *brd, GroupInfo *gi, int g)
{
    Group gr;
    int r, c;

    if (gi->size[g] > ANALYSIS_MAX_SIZE) return -1;

    gr.height = gi->bounds[g].q.r - gi->bounds[g].p.r;
    gr.width  = gi->bounds[g].q.c - gi->bounds[g].p.c;
    for (r = 0; r < gr.height; ++r)
    {
        gr.rows[r] = 0;
        for (c = 0; c < gr.width; ++c)
        {
            if ((*brd)[r + gi->bounds[g].p.r][c + gi->bounds[g].p.c] == g + 1)
            {
                GR_SET(&gr, r, c);
            }
        }
    }

    return group_nvalue(&gr);
}

/* Flood fill the board from a specific position,
   filling empty squares with group (gi->num_groups + 1). */
static void fill(Board *brd, GroupInfo *gi, int r, int c)
{
    Rect *rect;
    struct QueueEntry { int r, c; } queue[100];
    int pos, len, g;

    g = gi->num_groups;
    rect = &gi->bounds[g];
    ((*brd)[r][c]) = g + 1;
    len = pos = 0;
    queue[len++] = (struct QueueEntry) { r, c };
    for (pos = 0; pos < len; ++pos)
    {
        r = queue[pos].r;
        c = queue[pos].c;
        if (r < rect->p.r) rect->p.r = r;
        if (r > rect->q.r) rect->q.r = r;
        if (c < rect->p.c) rect->p.c = c;
        if (c > rect->q.c) rect->q.c = c;
        if (r > 0 && (*brd)[r - 1][c] == 0)
        {
            ((*brd)[r - 1][c]) = g + 1;
            queue[len++] = (struct QueueEntry) { r - 1, c };
        }
        if (r < 9 && (*brd)[r + 1][c] == 0)
        {
            ((*brd)[r + 1][c]) = g + 1;
            queue[len++] = (struct QueueEntry) { r + 1, c };
        }
        if (c > 0 && (*brd)[r][c - 1] == 0)
        {
            ((*brd)[r][c - 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r, c - 1};
        }
        if (c < 9 && (*brd)[r][c + 1] == 0)
        {
            ((*brd)[r][c + 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r, c + 1};
        }
    }
    gi->size[g] = len;
}

void analysis_initialize()
{
    /* does nothing right now (but must be called anyway) */
    analyis_initialized = true;
}

void analysis_identify_groups(Board *brd, GroupInfo *gi)
{
    int r, c;

    assert(analyis_initialized);

    gi->num_groups = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*brd)[r][c] == 0)
            {
                gi->first [gi->num_groups] = (Point) { r, c };
                gi->bounds[gi->num_groups] = (Rect) { { r, c }, { r, c } };
                fill(brd, gi, r, c);
                gi->bounds[gi->num_groups].q.r += 1;
                gi->bounds[gi->num_groups].q.c += 1;
                gi->num_groups += 1;
            }
        }
    }
}

void analysis_nim_values(Board *brd, GroupInfo *gi)
{
    int n, num_large, num_guessed;

    assert(analyis_initialized);

    num_large = num_guessed = 0;
    gi->nsum = 0;
    for (n = 0; n < gi->num_groups; ++n)
    {
        gi->nval[n] = nvalue(brd, gi, n);
        if (gi->nval[n] > 1) ++num_large;
        if (gi->nval[n] < 0)
        {
            gi->nsum ^= gi->size[n];
            ++num_guessed;
        }
        else
        {
            gi->nsum ^= gi->nval[n];
        }
    }
    gi->complete = num_guessed == 0;
    gi->winning  = (num_large == 0) ^ (gi->nsum != 0);
}

int analysis_value_moves_normal(Board *brd_in, Rect *moves, int *values)
{
    int move, num_moves;
    Board brd;
    GroupInfo gi;

    assert(analyis_initialized);

    num_moves = board_list_moves(brd_in, moves);
    for (move = 0; move < num_moves; ++move)
    {
        /* Construct the board after executing the i-th move */
        memcpy(brd, brd_in, sizeof(brd));
        board_flatten(&brd);
        board_fill(&brd, &moves[move], -1);
        analysis_identify_groups(&brd, &gi);
        analysis_nim_values(&brd, &gi);
        values[move] = (gi.complete ? 2 : 1) * (gi.winning ? -1 : +1);
    }

    return num_moves;
}

static void erase_trivial_groups(Board *brd, int *ones, int *twos)
{
    GroupInfo gi;
    int g, r, c;

    *ones = *twos = 0;

    board_flatten(brd);
    analysis_identify_groups(brd, &gi);
    for (g = 0; g < gi.num_groups; ++g)
    {
        if (gi.size[g] <= 3)
        {
            if (gi.size[g]&1) *ones ^= 1;
            if (gi.size[g]&2) *twos += 1;

            for (r = 0; r < 10; ++r)
            {
                for (c = 0; c < 10; ++c)
                {
                    if ((*brd)[r][c] == g + 1) (*brd)[r][c] = -1;
                }
            }
        }
    }
    board_clear_groups(brd);
}

/* Does the actual work of evaluating misere values of moves.

   Note: dynamically allocates a lot of stack space!
*/
static void value_moves_misere(
    Board *brd, Board *labels,
    int num_moves, Rect *moves, int *values,
    const int spaces, const int ones, const int twos )
{
    Mask mm[MAX_MOVES+1], *m;
    bool won[twos + 1][2][1<<spaces];
    int r, c, n;
    int one, two;

    /* Build move masks */
    for (n = 0; n < num_moves; ++n)
    {
        mm[n] = 0;
        for (r = 0; r < 10; ++r)
        {
            for (c = 0; c < 10; ++c)
            {
                if (r >= moves[n].p.r && r < moves[n].q.r &&
                    c >= moves[n].p.c && c < moves[n].q.c )
                {
                    if ((*labels)[r][c] == -1)
                    {
                        /* Move in one/two/three that was removed */
                        mm[n] = (Mask)-1;
                    }
                    else
                    {
                        mm[n] |= (1<<(*labels)[r][c]);
                    }
                }
            }
        }
    }
    mm[num_moves] = 0;

    /* Determine state of all subpositions */
    for (two = 0; two <= twos; ++two)
    {
        for (one = 0; one < 2; ++one)
        {
            for (n = 0; n < (1<<spaces); ++n)
            {
                if (two == 0 && one == 0 && n == 0) goto won;

                /* See if we can move to a losing position (then we win) */
                for (m = mm; *m != 0; ++m)
                {
                    if ((n&*m) != *m) continue;
                    if (!won[two][one][n^*m]) goto won;
                }
                if (one > 0 && !won[two][0][n]) goto won;
                if (two > 0 && !won[two-1][one][n]) goto won;
                if (two > 0 && !won[two-1][one^1][n]) goto won;

                /* No winning moves found */
                won[two][one][n] = false;
                continue;

            won:
                /* Winning move found */
                won[two][one][n] = true;
                continue;
            }
        }
    }

    /* Assign move values */
    for (n = 0; n < num_moves; ++n)
    {
        if (mm[n] != (Mask)-1)
        {
            values[n] = won[twos][ones][((1<<spaces)-1)^mm[n]] ? -3 : +3;
        }
        else
        {
            Board tmp;
            memcpy(&tmp, brd, sizeof(tmp));
            board_fill(&tmp, &moves[n], -1);
            erase_trivial_groups(&tmp, &one, &two);
            values[n] = won[two][one][(1<<spaces)-1] ? -3 : +3;
        }
    }
}

int analysis_value_moves_misere(Board *brd_in, Rect *moves, int *values)
{
    Board brd, labels;
    int num_moves, num_spaces;
    int ones, twos;
    int r, c;
    long long est_iter;

    assert(analyis_initialized);

    /* Build board, for optimization */
    memcpy(&brd, brd_in, sizeof(brd));

    /* Determine available moves */
    num_moves = board_list_moves(&brd, moves);

    /* Remove ones/two/threes */
    erase_trivial_groups(&brd, &ones, &twos);

    /* Determine number of spaces */
    num_spaces = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            labels[r][c] = brd[r][c] ? -1 : num_spaces++;
        }
    }

    /* Estimate required number of iterations */
    if (num_spaces > 40)
    {
        est_iter = 999999999999999999LL;
    }
    else
    {
        est_iter = (2LL*(twos + 1)*(num_moves + 3)) << num_spaces;
    }

    fprintf(stderr, "Est. iterations: %lld", est_iter);

    if (est_iter > MINIMAX_MAX_ITERATIONS) return -1;

    value_moves_misere( brd_in, &labels,
                        num_moves, moves, values,
                        num_spaces, ones, twos );

    return num_moves;
}
