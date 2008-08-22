#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "Board.h"
#include <stdint.h>

#define MAX_GROUPS            50    /* max. number of groups on a board */
#define MAX_MOVES           3025    /* max. number of moves possible */

#ifndef LOCAL   /* competition settings */
#define ANALYSIS_MAX_SIZE    20    /* max. size of group for in-depth analysis */
#else           /* local settings */
#define ANALYSIS_MAX_SIZE    27    /* max. size of group for in-depth analysis */
#endif

/* Minimum size of shapes to cache */
#define ANALYSIS_MIN_CACHE_SIZE 16

typedef signed char NV;             /* a nim-value */
typedef uint32_t Mask;              /* bitmask for analysis */
typedef uint32_t NVSet;             /* set of nim-values */

/* Global variables used internally */
NV memo[1<<ANALYSIS_MAX_SIZE];
Mask moves[MAX_MOVES+1];
int skip[MAX_MOVES];

typedef struct GroupInfo
{
    int num_groups;             /* number of groups identified */
    Point first[MAX_GROUPS];    /* one point that's part of the group */
    int   size[MAX_GROUPS];     /* number of fields in the group */
    Rect  bounds[MAX_GROUPS];   /* minimal bounding rectangle of the group */
    NV    nval[MAX_GROUPS];     /* nim-value of group */
} GroupInfo;

/* Initialize the analysis module */
void analysis_initialize();

/* Identifies groups in the board. Input must be a truncated board.
   Board is modified so that squares belonging to group 'g' are marked with
   byte g+1. Does not fill in nim values. */
void analysis_identify_groups(Board *brd, GroupInfo *gi);

/* Assign nim-values to groups (if possible). */
void analysis_nim_values(Board *brd, GroupInfo *gi);

#endif /* ndef ANALYSIS_H */
