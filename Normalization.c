#include "Group.h"
#include <stdlib.h>
#include <assert.h>

static void mirror_horizontal(Group *src, Group *dst)
{
    int r;
    uint16_t row, mid;

    dst->height = src->height;
    dst->width  = src->width;
    for (r = 0; r < src->height; ++r)
    {
        row = src->rows[r];
        row = ((row & 0x01F) << 5) | ((row & 0x3E0) >> 5);
        mid = row & 0x084;
        row = ((row & 0x063) << 3) | ((row & 0x318) >> 3);
        row = ((row & 0x129) << 1) | ((row & 0x252) >> 1);
        dst->rows[r] = ((row | mid) >> (10 - src->width));
    }
}

static void mirror_vertical(Group *src, Group *dst)
{
    int r1, r2;

    r1 = 0;
    r2 = src->height;
    while (r2 > 0) dst->rows[r1++] = src->rows[--r2];
    dst->height = src->height;
    dst->width  = src->width;
}

static void mirror_diagonal(Group *src, Group *dst)
{
    int r, c;

    dst->height = src->width;
    dst->width  = src->height;

    for (r = 0; r < dst->height; ++r)
    {
        dst->rows[r] = 0;
        for (c = 0; c < dst->width; ++c)
        {
            dst->rows[r] |= ((src->rows[c]>>r)&1)<<c;
        }
    }
}

static int group_cmp_rows(const Group *gr1, const Group *gr2)
{
    int r, diff;
    /* assert(gr1->height == gr2->height && gr1->width == gr2->width); */
    for (r = 0; r < gr1->height; ++r)
    {
        diff = gr1->rows[r] - gr2->rows[r];
        if (diff != 0) return diff;
    }
    return 0;
}

#if 0
static int group_cmp(const void *a, const void *b)
{
    const Group *gr1 = a, *gr2 = b;
    int diff = gr1->height - gr2->height;
    if (diff != 0) return diff;
    /* assert(a.width == b.width); */
    return group_cmp_rows(gr1, gr2);
}
#endif

/* Normalizes a group with respect to horizontal/vertical reflections
   (but not rotations!) */
static void group_normalize_half(Group *gr)
{
    Group alt1, alt2;

    mirror_vertical(gr, &alt1);
    if (group_cmp_rows(gr, &alt1) > 0) *gr = alt1;

    mirror_horizontal(&alt1, &alt2);
    if (group_cmp_rows(gr, &alt2) > 0) *gr = alt2;

    mirror_vertical(&alt2, &alt1);
    if (group_cmp_rows(gr, &alt1) > 0) *gr = alt1;
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
        if (group_cmp_rows(gr, &alt) > 0) *gr = alt;
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

/* Generates alternatives for the group alt[0] and stores them in alt[1] to
   alt[n-1], and returns n.
   (NB. these alternatives are not guaranteed to be distinct) */
int group_alternatives(Group *alts)
{
    mirror_vertical   (&alts[0], &alts[1]);
    mirror_horizontal (&alts[1], &alts[2]);
    mirror_vertical   (&alts[2], &alts[3]);
    mirror_diagonal   (&alts[3], &alts[4]);
    mirror_vertical   (&alts[4], &alts[5]);
    mirror_horizontal (&alts[5], &alts[6]);
    mirror_vertical   (&alts[6], &alts[7]);
    return 8;
}
