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

/* Determines wether the space at (r,c) is a singleton space; i.e.
   it's completely surrounded by blocked squares or the edge of the board. */
static bool singleton(Board *brd, int r, int c)
{
    assert((*brd)[r][c] == 0);
    return (r == 0 || (*brd)[r - 1][c] != 0) &&
           (c == 0 || (*brd)[r][c - 1] != 0) &&
           (r == 9 || (*brd)[r + 1][c] != 0) &&
           (c == 9 || (*brd)[r][c + 1] != 0);
}

void remove_singleton_pairs(Board *brd)
{
    int r, c, last_r = -1, last_c = -1;

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*brd)[r][c] == 0 && singleton(brd, r, c))
            {
                if (last_r >= 0 && last_c >= 0)
                {
                    (*brd)[r][c] = -2;
                    (*brd)[last_r][last_c] = -2;
                    last_r = last_c = -1;
                }
                else
                {
                    last_r = r;
                    last_c = c;
                }
            }
        }
    }
}

int analysis_value_moves_misere(Board *brd_in, Rect *moves, int *values)
{
    static Mask mm[MAX_MOVES+1];
    static bool won[1<<MINIMAX_MAX_SIZE];

    Board brd, labels;
    Mask *m;
    int n, num_moves, num_spaces;
    int r, c;

    assert(analyis_initialized);

    /* Build board, canceling out singletons */
    memcpy(&brd, brd_in, sizeof(brd));
    remove_singleton_pairs(&brd);

    /* Determine number of spaces */
    num_spaces = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            labels[r][c] = brd[r][c] ? -1 : num_spaces++;
        }
    }

    /* Determine available moves */
    num_moves = board_list_moves(&brd, moves);

    if (num_moves == 0)
    {
        /* Only even number of singletons left -- any move is winning! */
        num_moves = board_list_moves(brd_in, moves);
        for (n = 0; n < num_moves; ++n) values[n] = +1;
        return num_moves;
    }

    /* Check wether search is feasible */
    if (num_spaces > MINIMAX_MAX_SIZE)
    {
        return -1;
    }

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
                    assert(labels[r][c] != -1);
                    mm[n] |= (1<<labels[r][c]);
                }
            }
        }
    }
    mm[num_moves] = 0;

    /* Determine state of all subpositions */
    won[0] = true;
    for (n = 1; n < (1<<num_spaces); ++n)
    {
        won[n] = false;
        for (m = mm; *m != 0; ++m)
        {
            if ((n&*m) != *m) continue;
            if (!won[n^*m])
            {
                won[n] = true;
                break;
            }
        }
    }

    /* Assign move values */
    for (n = 0; n < num_moves; ++n)
    {
        values[n] = won[((1<<num_spaces)-1)^mm[n]] ? -2 : +2;
    }

    return num_moves;
}
