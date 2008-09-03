#include "Analysis.h"
#include "HashTable.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* nvalue cache -- to avoid recomputing nvalues for large areas */
HashTable *nvcache;

typedef struct NVCacheEntry
{
    uint8_t board[13];
    signed char nvalue;
} NVCacheEntry;

HashTable *nvcache;


/* worker function -- defined elsewhere */
int nvalue_work(int grp_size);

/* Determine the nim value for the given group. */
static int nvalue_old(Board *brd, GroupInfo *gi, int g)
{
    Mask mask, *move;
    int num_moves, result;
    Rect rect;
    NVCacheEntry *nvce;

    if (gi->size[g] > ANALYSIS_MAX_SIZE) return -1;
    assert(gi->size[g] > 0);

    if (gi->size[g] < ANALYSIS_MIN_CACHE_SIZE)
    {
        nvce = NULL;
    }
    else
    {
        int r, c;
        NVCacheEntry *old;

        nvce = malloc(sizeof(NVCacheEntry));
        assert(nvce != NULL);
        memset(nvce->board, 0, sizeof(nvce->board));
        for (r = 0; r < 10; ++r)
        {
            for (c = 0; c < 10; ++c)
            {
                if ((*brd)[r][c] == g + 1)
                {
                    nvce->board[(10*r + c)/8] |= 1<<((10*r + c)%8);
                }
            }
        }
        old = HT_get_or_set(nvcache, nvce->board, nvce);
        if (old != NULL)
        {
            free(nvce);
            return old->nvalue;
        }
    }

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
    result = nvalue_work(gi->size[g]);

    if (nvce != NULL) nvce->nvalue = result;

    return result;
}

typedef struct NewCacheEntry
{
    Board board;
    NV nvalue;
} NewCacheEntry;

HashTable *new_cache;

uint32_t hash_data[100] = {
0x55fe0666u, 0xc5edfcbau, 0x32a6514fu, 0xad393fd2u, 0x6dacdb30u,
0x5150512du, 0x3a18390cu, 0x57206b3cu, 0xd6b647f2u, 0xe67e251du,
0xc62d62b2u, 0x6b38e425u, 0x135eadfbu, 0x9b0e51a1u, 0x1de68522u,
0x0ae219c3u, 0xd0d9d33fu, 0xc362ad1bu, 0x0f7df072u, 0x7e446bc7u,
0xc69a695fu, 0x19df6e11u, 0x141c6873u, 0x65ed38b9u, 0xd8cb1262u,
0x6c1c823fu, 0x8e44dda7u, 0x14573ec0u, 0x2712c1edu, 0xe0eec4a5u,
0x20a27fa6u, 0x0f278993u, 0xd61a08bau, 0xa4429702u, 0xa346b834u,
0x6f1126bcu, 0xde3774d0u, 0xb299cbd3u, 0x3ced3879u, 0xea580437u,
0xd0b836eau, 0xf507ede6u, 0x521a2d84u, 0x24485a55u, 0xbdc7adfau,
0x43efbe8fu, 0xbee93d6cu, 0x9d92db88u, 0xaf382df3u, 0x4e98f38cu,
0x6bdf1070u, 0xca7c2cbau, 0x5fe3201au, 0xf6a51375u, 0x8affd79cu,
0x6655a5b4u, 0xa9590d76u, 0xaa7c022au, 0x3ba873c5u, 0xb02c7344u,
0x936b0b3du, 0xe289e011u, 0x85c9908du, 0xe3df0d59u, 0x31487bcbu,
0xab7e5971u, 0x017d62afu, 0x0da2208au, 0x561b1589u, 0x42e8a9c7u,
0x0634138eu, 0x7f9e6d82u, 0x83fb95e7u, 0xeb1dfd61u, 0xffae4105u,
0x86b073eau, 0x6734722cu, 0xe2f99f99u, 0x4fde9050u, 0xd123bf6eu,
0x1ac90da1u, 0x2f5d555fu, 0x4fec7a57u, 0x15f94b4eu, 0x2fde3eceu,
0xac0eba14u, 0xd59b76c7u, 0xb173bd51u, 0xc06e8a18u, 0x3bd9d3fbu,
0x999b2495u, 0x3d4452e8u, 0x402bbcb0u, 0xb16fcf73u, 0x43f63af6u,
0x53c693a7u, 0x42bccbf6u, 0x928080c5u, 0xde9183ceu, 0x42ba211bu };

static uint32_t hash_board(void const *data, size_t len)
{
    uint32_t h = 0;
    size_t n;

    for (n = 0; n < len; ++n)
    {
        h ^= ((char*)data)[n] * hash_data[n];
    }
    return h;
}

static void mirror_horizontal(Board *brd, int h, int w, Board *dst)
{
    int r, c1, c2;

    for (r = 0; r < h; ++r)
    {
        c2 = w;
        for (c1 = 0; c1 < w; ++c1)
        {
            (*dst)[r][c1] = (*brd)[r][--c2];
        }
    }
}

static void mirror_vertical(Board *brd, int h, int w, Board *dst)
{
    int r1, r2, c;

    for (c = 0; c < w; ++c)
    {
        r2 = h;
        for (r1 = 0; r1 < h; ++r1)
        {
            (*dst)[r1][c] = (*brd)[--r2][c];
        }
    }
}

static void mirror_diagonal(Board *brd, int h, int w, Board *dst)
{
    int r, c;

    for (r = 0; r < w; ++r)
    {
        for (c = 0; c < h; ++c)
        {
            (*dst)[r][c] = (*brd)[c][r];
        }
    }
}

static void store_board(Board *brd, NV nval)
{
    NewCacheEntry *e, *old;

    e = malloc(sizeof(NewCacheEntry));
    memcpy(&(e->board), brd, sizeof(Board));
    e->nvalue = nval;

    old = NULL;
    HT_set(new_cache, e, e, NULL, (void**)&old);
    if (old != NULL) free(old);
}

static void store_boards(Board *brd, int h, int w, NV nval)
{
    Board a, b;

    /* Option 1 */
    store_board(brd, nval);

    /* Option 2 */
    memset(&b, -1, sizeof(Board));
    mirror_horizontal(brd, h, w, &b);
    store_board(&b, nval);

    /* Option 3 */
    memset(&a, -1, sizeof(Board));
    mirror_vertical(&b, h, w, &a);
    store_board(&a, nval);

    /* Option 4 */
    mirror_horizontal(&a, h, w, &b);
    store_board(&b, nval);

    /* Option 5 */
    memset(&a, -1, sizeof(Board));
    mirror_diagonal(&b, h, w, &a);
    store_board(&a, nval);

    /* Option 6 */
    memset(&b, -1, sizeof(Board));
    mirror_horizontal(&a, w, h, &b);
    store_board(&b, nval);

    /* Option 7 */
    mirror_vertical(&b, w, h, &a);
    store_board(&a, nval);

    /* Option 8 */
    mirror_horizontal(&a, w, h, &b);
    store_board(&b, nval);
}

/* Determine the nim value for the given group. */
static int nvalue(Board *brd, GroupInfo *gi, int g)
{
    Board neg;
    int r, c, h, w;
    NV nval, child_nval;
    Rect moves[MAX_MOVES];
    int num_moves, n, child_g;
    GroupInfo child_gi;
    unsigned nvals;

    memset(&neg, -1, sizeof(Board));
    h = gi->bounds[g].q.r - gi->bounds[g].p.r;
    w = gi->bounds[g].q.c - gi->bounds[g].p.c;
    for (r = 0; r < h; ++r)
    {
        for (c = 0; c < w; ++c)
        {
            if ((*brd)[r + gi->bounds[g].p.r][c + gi->bounds[g].p.c] == g + 1) neg[r][c] = 0;
        }
    }

    /* Look up in hash table */
    NewCacheEntry *e = HT_get(new_cache, &neg);
    if (e != NULL)
    {
        return e->nvalue;
    }

    /* Consider all possible moves */
    num_moves = board_list_moves(&neg, moves);
    nvals = 0;
    nval = 0;
    for (n = 0; n < num_moves; ++n)
    {
        board_fill(&neg, &moves[n], 127);
        analysis_identify_groups(&neg, &child_gi);
 /*       if (child_gi.num_groups == 1 && child_gi.size[0] < nval) goto skip;*/

        child_nval = 0;
        for (child_g = 0; child_g < child_gi.num_groups; ++child_g)
        {
            child_nval ^= nvalue(&neg, &child_gi, child_g);
        }
        nvals |= (1<<child_nval);

        /* Update my nval */
        while (nvals & (1u << (int)nval)) ++nval;

    skip:
        for (r = 0; r < h; ++r)
        {
            for (c = 0; c < w; ++c)
            {
                if (neg[r][c] > 0) neg[r][c] = 0;
            }
        }

    }

    store_boards(&neg, h, w, nval);

    return nval;
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
        if (r > 0 && (*brd)[r - 1][c    ] == 0)
        {
            ((*brd)[r - 1][c    ]) = g + 1;
            queue[len++] = (struct QueueEntry) { r - 1, c    };
        }
        if (r < 9 && (*brd)[r + 1][c    ] == 0)
        {
            ((*brd)[r + 1][c    ]) = g + 1;
            queue[len++] = (struct QueueEntry) { r + 1, c    };
        }
        if (c > 0 && (*brd)[r    ][c - 1] == 0)
        {
            ((*brd)[r    ][c - 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r    , c - 1};
        }
        if (c < 9 && (*brd)[r    ][c + 1] == 0)
        {
            ((*brd)[r    ][c + 1]) = g + 1;
            queue[len++] = (struct QueueEntry) { r    , c + 1};
        }
    }
    gi->size[g] = len;
}

void analysis_initialize()
{
    NVCacheEntry dummy;
    (void)dummy;
    assert(sizeof(dummy.board) == 13);
    nvcache = HT_create(sizeof(dummy.board), 1000007, NULL);
    assert(nvcache != NULL);

    new_cache = HT_create(sizeof(Board), 3000007, hash_board);
    assert(new_cache != NULL);
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
    int n;
    for (n = 0; n < gi->num_groups; ++n)
    {
        gi->nval[n] = nvalue(brd, gi, n);
    }
}

int analysis_value_moves(Board *brd_in, Rect *moves, int *values)
{
    int move, num_moves;
    Board brd;
    GroupInfo gi;

    num_moves = board_list_moves(brd_in, moves);
    for (move = 0; move < num_moves; ++move)
    {
        int group, num_ones, nsum, guessed;

        /* Construct the board after executing the i-th move */
        memcpy(brd, brd_in, sizeof(brd));
        board_flatten(&brd);
        board_fill(&brd, &moves[move], -1);
        analysis_identify_groups(&brd, &gi);
        analysis_nim_values(&brd, &gi);

        /* Determine status of the board by computing its nim-sum */
        num_ones = 0;
        nsum     = 0;
        guessed  = 0;

        for (group = 0; group < gi.num_groups; ++group)
        {
            if (gi.size[group] == 1) ++num_ones;
            if (gi.nval[group] < 0)
            {
                nsum ^= gi.size[group];
                ++guessed;
            }
            else
            {
                nsum ^= gi.nval[group];
            }
        }

        if (num_ones == gi.num_groups)
        {
            /* Only groups with nim value 1 left */
            values[move] = (nsum == 0) ? -1 : +1;
        }
        else
        {
            values[move] = (nsum == 0) ? +1 : -1;
        }

        if (!guessed) values[move] *= 2;
    }

    return num_moves;
}
