#ifndef GROUP_H_INCLUDED
#define GROUP_H_INCLUDED

#include <stdint.h>

#define MAX_GROUPS            50    /* max. number of groups on a board */

#ifndef ANALYSIS_MAX_SIZE
#define ANALYSIS_MAX_SIZE     18    /* max. size of group for in-depth analysis */
#endif

typedef signed char NV;             /* a nim-value */
typedef uint32_t Mask;              /* bitmask for analysis */
typedef uint32_t NVSet;             /* set of nim-values */

/* Models a single group, enclosed in the rectangle (0,0)-(height,width)
   Every field in this rectangle is set to 1 if it is part of the group; fields
   outside the rectangle may have random values. Fields must be connected. */
typedef struct Group
{
    int height, width;
    Board board;
    int pop;
} Group;

NV group_nvalue(Group *gr);
void group_print(const Group *gr);
void group_normalize(Group *gr);
void group_isolate(Group *src, int r, int c, Group *dst);

#endif /* ndef GROUP_H_INCLUDED */
