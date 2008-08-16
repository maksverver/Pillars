#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

typedef signed char Field;
typedef Field Board[10][10];

/* Point on the 10x10 grid. Required: 0 <= r,c <= 10 */
typedef struct Point { int r, c; } Point;

/* Non-empty rectangle between (p.x,p.y)-(q.x,q.y) (exclusive)
   Required: p.x < q.x && p.y < q.y */
typedef struct Rect { Point p, q; } Rect;

bool board_decode_full(Board *board, const char *str);
bool board_decode_short(Board *board, const char *str);
void board_encode_full(Board *board, char buf[201]);
void board_encode_short(Board *board, char buf[26]);
void board_truncate(Board *board);
bool board_is_valid_move(Board *board, Rect *rect);
void board_fill(Board *board, Rect *rect, int val);
void board_print(Board *board, FILE *fp);

#endif /* ndef BOARD_H_INCLUDED */
