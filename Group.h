#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include "Board.h"
#include <stdint.h>

#define MAX_GROUPS            50    /* max. number of groups on a board */

#define GR_GET(gr, r, c) (((gr)->rows[(int)(r)] >> (int)(c))&1)
#define GR_SET(gr, r, c) ((void)((gr)->rows[(int)(r)] |= (1<<(int)(c))))
#define GR_CLR(gr, r, c) ((gr)->rows[(int)(r)] &= ~(1<<(int)(c)))
#define GR_TOG(gr, r, c) ((gr)->rows[(int)(r)] ^= (1<<(int)(c)))

typedef signed char NV;             /* a nim-value */
typedef uint32_t Mask;              /* bitmask for analysis */
typedef uint32_t NVSet;             /* set of nim-values */

/* Models a single group, enclosed in the rectangle (0,0)-(height,width)
   Every field in this rectangle is set to 1 if it is part of the group; fields
   outside the rectangle may have random values. Fields must be connected. */
typedef struct Group
{
    uint16_t height, width;
    uint16_t rows[10];
} Group;

NV group_nvalue(Group *gr);
void group_print(const Group *gr);
void group_isolate(Group *src, int r, int c, Group *dst);

/* Defined in Normalization.c */
void group_normalize(Group *gr);
int group_alternatives(Group gr[8]);

#endif /* ndef GROUP_H_INCLUDED */
