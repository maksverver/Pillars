#include "Analysis.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* worker function -- defined elsewhere */
int nvalue_new_work(int grp_size);

/* Determine the nim value for the given group. */
static int nvalue_new(Board *brd, GroupInfo *gi, int g)
{
    Mask mask, *move;
    int num_moves;
    Rect rect;

    if (gi->size[g] > ANALYSIS_MAX_SIZE) return -1;
    assert(gi->size[g] > 0);

    /* Figure out all valid moves */
    num_moves = 0;
    for (rect.p.r = 0; rect.p.r < 10; ++rect.p.r)
    {
        for (rect.p.c = 0; rect.p.c < 10; ++rect.p.c)
        {
            if ((*brd)[rect.p.r][rect.p.c] != g + 1) continue;

            for (rect.q.r = rect.p.r + 1; rect.q.r <= 10; ++rect.q.r)
            {
                for (rect.q.c = rect.p.c + 1; rect.q.c <= 10; ++rect.q.c)
                {
                    int r, c;

                    for (r = rect.p.r; r < rect.q.r; ++r)
                    {
                        for (c = rect.p.c; c < rect.q.c; ++ c)
                        {
                            if ((*brd)[r][c] != g + 1) goto invalid;
                        }
                    }

                    mask = 0;
                    for (r = 0; r < 10; ++r)
                    {
                        for (c = 0; c < 10; ++ c)
                        {
                            if ((*brd)[r][c] != g + 1) continue;
                            mask = 2*mask + ( r >= rect.p.r && r < rect.q.r &&
                                              c >= rect.p.c && c < rect.q.c );
                        }
                    }
                    assert(num_moves < MAX_MOVES);
                    moves[num_moves++] = mask;
                invalid: continue;
                }
            }
        }
    }
    moves[num_moves] = 0;   /* mark end of moves */

    /* Calculate move skip data */
    for (move = moves; *move != 0; ++move)
    {
        skip[move - moves] = 1;
        while ((move[skip[move - moves]]&*move) == *move) ++skip[move - moves];
    }

    /* Calculate nim value (this takes the most time) */
    return nvalue_new_work(gi->size[g]);
}

/* Flood fill the board from a specific position,
   filling empty squares with group (gi->num_groups + 1). */
static int fill(Board *brd, GroupInfo *gi, int r, int c)
{
    Rect *rect;
    if (r < 0 || r >= 10 || c < 0 || c >= 10 || (*brd)[r][c] != 0) return 0;
    (*brd)[r][c] = gi->num_groups + 1;
    rect = &gi->bounds[gi->num_groups];
    if (r < rect->p.r) rect->p.r = r;
    if (r > rect->q.r) rect->q.r = r;
    if (c < rect->p.c) rect->p.c = c;
    if (c > rect->q.c) rect->q.c = c;
    return 1 + fill(brd, gi, r - 1, c    )
             + fill(brd, gi, r + 1, c    )
             + fill(brd, gi, r    , c - 1)
             + fill(brd, gi, r    , c + 1);
}

void analysis_initialize()
{
    /* Nothing needed */
}

void analysis_identify_groups(Board *brd, GroupInfo *gi)
{
    int r, c;

    gi->num_groups = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*brd)[r][c] == 0)
            {
                gi->first [gi->num_groups] = (Point) { r, c };
                gi->bounds[gi->num_groups] = (Rect) { { r, c }, { r, c } };
                gi->size  [gi->num_groups] = fill(brd, gi, r, c);
                gi->bounds[gi->num_groups].q.r += 1;
                gi->bounds[gi->num_groups].q.c += 1;
                gi->num_groups += 1;
            }
        }
    }
}

void analysis_nim_values(Board *brd, GroupInfo *gi)
{
    int n;
    for (n = 0; n < gi->num_groups; ++n)
    {
        gi->nval[n] = nvalue_new(brd, gi, n);
    }
}
