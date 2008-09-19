#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include "Board.h"
#include <stdint.h>

#define MAX_GROUPS            50    /* max. number of groups on a board */

#ifndef ANALYSIS_MAX_SIZE
#define ANALYSIS_MAX_SIZE     18    /* max. size of group for in-depth analysis */
#endif

#ifndef MINIMAX_MAX_SIZE
                                    /* max. number of open fields to perform
                                       minimax search on. */
#define MINIMAX_MAX_SIZE (ANALYSIS_MAX_SIZE + 2)
#endif

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
    int height, width;
    uint16_t rows[10];
} Group;

NV group_nvalue(Group *gr);
void group_print(const Group *gr);
void group_normalize(Group *gr);
void group_isolate(Group *src, int r, int c, Group *dst);

#endif /* ndef GROUP_H_INCLUDED */
