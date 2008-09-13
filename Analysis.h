#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "Board.h"
#include "Group.h"

typedef struct GroupInfo
{
    int num_groups;             /* number of groups identified */
    Point first[MAX_GROUPS];    /* one point that's part of the group */
    int   size[MAX_GROUPS];     /* number of fields in the group */
    Rect  bounds[MAX_GROUPS];   /* minimal bounding rectangle of the group */
    NV    nval[MAX_GROUPS];     /* nim-value of group */

    bool  complete;             /* true if all nim values of groups are known */
    int   nsum;                 /* nim-value of board (guessed if incomplete) */
    int   winning;              /* is board winning? (guessed if incomplete) */
} GroupInfo;

/* Initialize the analysis module */
void analysis_initialize();

/* Identifies groups in the board. Input must be a flattened board.
   Board is modified so that squares belonging to group 'g' are marked with
   byte g+1. Does not fill in nim values. */
void analysis_identify_groups(Board *brd, GroupInfo *gi);

/* Assign nim-values to groups (if possible).
   Initializes gi->nval, gi->complete, gi->board_nval and gi->board_winning. */
void analysis_nim_values(Board *brd, GroupInfo *gi);

/* Analyzes the board and the possible moves, and values each move,
   with an integer:
    +2  known to be winning
    +1  guessed to be winning
    -1  guessed to be losing
    -2  known to be losing

   Moves are written to the moves array, values are written to the values
   array, and the number of moves available is returned. (The board provided
   is not modified.)
*/
int analysis_value_moves(Board *brd, Rect *moves, int *values);

#endif /* ndef ANALYSIS_H */
