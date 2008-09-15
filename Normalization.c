#include "Group.h"
#include <assert.h>

static void mirror_horizontal(Group *src, Group *dst)
{
    int r, c1, c2;

    dst->height = src->height;
    dst->width  = src->width;
    for (r = 0; r < src->height; ++r)
    {
        c1 = 0;
        c2 = src->width;
        while (c2 > 0) dst->board[r][c1++] = src->board[r][--c2];
    }
    dst->pop = src->pop;
}

static void mirror_vertical(Group *src, Group *dst)
{
    int r1, r2, c;

    dst->height = src->height;
    dst->width  = src->width;

    r1 = 0;
    r2 = src->height;
    while (r2 > 0)
    {
        --r2;
        for (c = 0; c < src->width; ++c) dst->board[r1][c] = src->board[r2][c];
        ++r1;
    }
    dst->pop = src->pop;
}

static void mirror_diagonal(Group *src, Group *dst)
{
    int r, c;

    dst->height = src->width;
    dst->width  = src->height;

    for (r = 0; r < dst->height; ++r)
    {
        for (c = 0; c < dst->width; ++c)
        {
            dst->board[r][c] = src->board[c][r];
        }
    }
    dst->pop = src->pop;
}

static int group_cmp(Group *gr1, Group *gr2)
{
    int r, c;
    assert(gr1->height == gr2->height && gr1->width == gr2->width);
    for (r = 0; r < gr1->height; ++r)
    {
        for (c = 0; c < gr1->width; ++c)
        {
            if (gr1->board[r][c] != gr2->board[r][c])
            {
                return gr1->board[r][c] - gr2->board[r][c];
            }
        }
    }
    return 0;
}

/* Normalizes a group with respect to horizontal/vertical reflections
   (but not rotations!) */
static void group_normalize_half(Group *gr)
{
    Group alt1, alt2;

    mirror_horizontal(gr, &alt1);
    if (group_cmp(gr, &alt1) > 0) *gr = alt1;

    mirror_vertical(&alt1, &alt2);
    if (group_cmp(gr, &alt2) > 0) *gr = alt2;

    mirror_horizontal(&alt2, &alt1);
    if (group_cmp(gr, &alt1) > 0) *gr = alt1;
}

/* Normalizes a group with respect to rotations/reflections.
   Note that this does not translate the group; the group is already assumed
   to be in its bounding rectangle! */
void group_normalize(Group *gr)
{
    Group alt;

    if (gr->width == gr->height)
    {
        group_normalize_half(gr);
        mirror_diagonal(gr, &alt);
        group_normalize_half(&alt);
        if (group_cmp(gr, &alt) > 0) *gr = alt;
    }
    else
    {
        if (gr->width < gr->height)
        {
            mirror_diagonal(gr, &alt);
            *gr = alt;
        }
        group_normalize_half(gr);
    }
}
