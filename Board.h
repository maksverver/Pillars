#ifndef BOARD_H_INCLUDED
#define BOARD_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

#define NUM_BOARDS 3628800      /* == factorial of 10 */
#define MAX_MOVES     1090

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
void board_flatten(Board *board);
void board_clear(Board *board);
bool board_is_valid_move(Board *board, Rect *rect);
void board_fill(Board *board, Rect *rect, int val);
int board_empty_area(Board *board);
void board_construct(Board *board, int number);
void board_print(Board *board, FILE *fp);
int board_list_moves(Board *board, Rect *moves);
bool board_get_move(Board *brd, Rect *move, int n);

bool point_decode(Point *p, const char *buf);
void point_encode(const Point *p, char buf[3]);

bool rect_decode(Rect *r, const char *buf);
void rect_encode(const Rect *r, char buf[5]);

#endif /* ndef BOARD_H_INCLUDED */
