#include "Analysis.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*  Memos mapping bitmask representations of fields of given sizes
    memoXY corresponds to a grid of height X and width Y.
    Maximum value appears to be 24.
*/
static signed char memo55[1<<( 5*5)];          /* 32 MB */
static signed char memo46[1<<( 4*6)];          /* 16 MB */
static signed char memo38[1<<( 3*8)];          /* 16 MB */
static signed char memo2A[1<<(2*10)];          /*  1 MB */

/* A bitmask representation for a part of the board.
   Ones correspond to open squares, zeroes with blocked squares.
   Mapping is such that in a rectange of height X and width Y,
   field (r,c) corresponds with bit (Y*r + c).
*/
typedef uint32_t Mask;

/* Swap two integers */
static __inline__ void swap_int(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static Mask make_mask(Board *board, int g, Rect *rect)
{
    int r, c;
    Mask m;

    m = 0;
    for (r = rect->p.r; r < rect->q.r; ++r)
    {
        for (c = rect->p.c; c < rect->q.c; ++c)
        {
            m <<= 1;
            if ((*board)[r][c] == (g + 1)) m |= 1;
        }
    }
    return m;
}

static Mask make_mask_flipped(Board *board, int g, Rect *rect)
{
    int r, c;
    Mask m;

    m = 0;
    for (c = rect->p.c; c < rect->q.c; ++c)
    {
        for (r = rect->p.r; r < rect->q.r; ++r)
        {
            m <<= 1;
            if ((*board)[r][c] == (g + 1)) m |= 1;
        }
    }
    return m;
}

/* Adjust the width (if w == true) or height (if w == false) to length
   "len" by changing the bottomleft point of the rectangle, and possible
   the topleft point as well, if this is necessary to make the rectangle
   fit in the 10x10 grid. */
static void adjust_side(Rect *rect, bool w, int len)
{
    if (w)
    {
        rect->q.c = rect->p.c + len;
        if (rect->q.c > 10)
        {
            rect->p.c = 10 - len;
            rect->q.c = 10;
        }
    }
    else
    {
        rect->q.r = rect->p.r + len;
        if (rect->q.r > 10)
        {
            rect->p.r = 10 - len;
            rect->q.r = 10;
        }
    }
}

static bool read_memo(const char *filename, signed char *memo, size_t size)
{
    FILE *fp;
    size_t nread;

    fp = fopen(filename, "rb");
    if (!fp) return false;
    nread = fread(memo, sizeof(*memo), size, fp);
    fclose(fp);
    return nread == size;
}

static bool write_memo(const char *filename, signed char *memo, size_t size)
{
    FILE *fp;
    size_t nwritten;

    fp = fopen(filename, "wb");
    if (!fp) return false;
    nwritten = fwrite(memo, sizeof(*memo), size, fp);
    fclose(fp);
    return nwritten == size;
}

static void init_memo(const char *filename, signed char *memo, int height, int width)
{
    Mask mask, ma, mb, mc, md;
    int r1, c1, r2, c2, nval;
    unsigned nvals;

    if (read_memo(filename, memo, (size_t)1 << height*width)) return;

    memset(memo, -1, (size_t)height*width);
    for (mask = 0; mask < ((Mask)1 << height*width); ++mask)
    {
        /* Try all moves */
        nvals = 0;

        ma = 1;
        for (r1 = 0; r1 < height; ++r1)
        {
            mb = ma;
            for (c1 = 0; c1 < width; ++c1)
            {
                if (mask&mb)
                {
                    mc = mb;
                    for (r2 = r1; r2 < height; ++r2)
                    {
                        md = mc;
                        for (c2 = c1; c2 < width; ++c2)
                        {
                            nvals |= (1<<memo[mask^md]);
                            md |= (md << 1);
                            if ((mask&md) != md) break;
                        }
                        mc |= (mc << width);
                        if ((mask&mc) != mc) break;
                    }
                }
                mb <<= 1;
            }
            ma <<= width;
        }

        /* Find minimum excluded ordinal */
        nval = 0;
        while (nvals&(1<<nval)) ++nval;
        assert(nval < 32);
        memo[mask] = nval;
    }

    if (write_memo(filename, memo, (size_t)1 << height*width)) return;
}

/* Determine the nim value for the given group.
   (Or return -1 if this is not possible.)  */
static int nvalue(Board *brd, GroupInfo *gi, int g)
{
    int height, width;
    bool flipped;
    Rect rect;
    signed char *memo;

    rect   = gi->bounds[g];
    height = rect.q.r - rect.p.r;
    width  = rect.q.c - rect.p.c;
    flipped = width < height;
    if (flipped) swap_int(&width, &height);

    if (height <= 5 && width <= 5)
    {
        adjust_side(&rect,  flipped, 5);
        adjust_side(&rect, !flipped, 5);
        memo = memo55;
    }
    else
    if (height <= 4 && width <= 6)
    {
        adjust_side(&rect,  flipped, 4);
        adjust_side(&rect, !flipped, 6);
        memo = memo46;
    }
    else
    if (height <= 3 && width <= 8)
    {
        adjust_side(&rect,  flipped, 3);
        adjust_side(&rect, !flipped, 8);
        memo = memo38;
    }
    else
    if (height <= 2 && width <= 10)
    {
        adjust_side(&rect,  flipped,  2);
        adjust_side(&rect, !flipped, 10);
        memo = memo2A;
    }
    else
    {
        return -1;
    }

    return memo[flipped ? make_mask_flipped(brd, g, &rect)
                        : make_mask(brd, g, &rect) ];
}

/* Maximum number of fields in a group to allow in-depth analysis. */
#define ANALYSIS_MAX_FIELDS 27

/* Determine the nim value for the given group. */
static int nvalue_new(Board *brd, GroupInfo *gi, int g)
{
    static signed char memo[1 << ANALYSIS_MAX_FIELDS];
    Mask mask, *move, moves[2501];
    int r, c, num_moves, result;
    uint32_t nvals;
    Rect rect;

    if (gi->size[g] > ANALYSIS_MAX_FIELDS) return -1;
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
                    assert(num_moves < 2500);
                    moves[num_moves++] = mask;
                invalid: continue;
                }
            }
        }
    }
    moves[num_moves] = 0;   /* mark end of moves */

    /* memset(memo, -1, sizeof(*memo) << gi->size[g]); */
    for (mask = ((Mask)1 << gi->size[g]); mask-- > 0; )
    {
        nvals = 0;
        for (move = moves; *move != 0; ++move)
        {
            if (mask&*move) continue;
            /* assert(memo[mask|*move] >= 0); */
            nvals |= (1<<memo[mask|*move]);
        }
        result = 0;
        while ((nvals & (1 << result)) != 0) ++result;
        /* assert(result < 32); */
        memo[mask] = result;
    }
    assert(result == memo[0]);
    return result;
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
    init_memo("memo55.dat", memo55, 5,  5);
    init_memo("memo46.dat", memo46, 4,  6);
    init_memo("memo38.dat", memo38, 3,  8);
    init_memo("memo2A.dat", memo2A, 2, 10);
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
        gi->nval[n] = nvalue(brd, gi, n);
        gi->nval2[n] = nvalue_new(brd, gi, n);
    }
}
