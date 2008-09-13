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
    gr.pop    = 0;
    for (r = 0; r < gr.height; ++r)
    {
        for (c = 0; c < gr.width; ++c)
        {
            gr.board[r][c] = (*brd)[r + gi->bounds[g].p.r][c + gi->bounds[g].p.c] == g + 1;
            gr.pop += gr.board[r][c];
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

int analysis_value_moves(Board *brd_in, Rect *moves, int *values)
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
