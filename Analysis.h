#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "Board.h"
#include "Group.h"

#ifndef ANALYSIS_MAX_SIZE
        /* max. group size for normal-play analysis */
#define ANALYSIS_MAX_SIZE 20
#endif

#ifndef MINIMAX_MAX_SIZE
        /* max. combined size for misere-play analysis */
#define MINIMAX_MAX_SIZE  ANALYSIS_MAX_SIZE
#endif

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

   Assumes normal play (last player to move wins) unless only groups
   equivalent to nim-heaps of size 1 or 0 remain. This strategy is not a
   correct for misere play, but appears to work reasonably well when the real
   analysis (with analysis_value_moves_misere) is infeasible.
*/
int analysis_value_moves_normal(Board *brd, Rect *moves, int *values);

/* Alternative move valuation using minimax search.

   Each value is set to one of:
    +2: known to be winning (N-position)
     0: status unknown
    -2: known to be losing (P-position)

   Assumes misere play (last player to move loses!).

   If analysis is infeasible, -1 is returned, and the caller should use
   analysis_value_moves_normal instead.
*/
int analysis_value_moves_misere(Board *brd, Rect *moves, int *values);

#endif /* ndef ANALYSIS_H */
