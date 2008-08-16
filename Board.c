#include "Board.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static const char hexdigits[] = "0123456789ABCDEF";

static bool is_hex_string(const char *str)
{
    while (*str)
    {
        if (strchr(hexdigits, *str) == NULL) return false;
        str += 1;
    }
    return true;
}

bool board_decode_full(Board *brd, const char *str)
{
    int r, c, i;

    if (strlen(str) != 200 || !is_hex_string(str)) return false;

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            i = 16*(strchr(hexdigits, str[20*r + 2*c + 0]) - hexdigits)
                 + (strchr(hexdigits, str[20*r + 2*c + 1]) - hexdigits);
            if (i >= 128) i = -1 - (i^255);
            (*brd)[r][c] = i;
        }
    }

    return true;
}

bool board_decode_short(Board *brd, const char *str)
{
    int i, n, r, c;
    if (strlen(str) != 25 || !is_hex_string(str)) return false;

    r = c = 0;
    while (*str)
    {
        i = strchr(hexdigits, *str++) - hexdigits;
        for (n = 3; n >= 0; --n)
        {
            (*brd)[r][c++] = ((i>>n)&1) ? -1 : 0;
            if (c == 10) c = 0, r++;
        }
    }
    assert(r == 10 && c == 0);

    return true;
}

void board_encode_full(Board *brd, char buf[201])
{
    int r, c;

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            *buf++ = hexdigits[((*brd)[r][c] >> 4)&15];
            *buf++ = hexdigits[((*brd)[r][c] >> 0)&15];
        }
    }
    *buf = '\0';
}

void board_encode_short(Board *brd, char buf[26])
{
    int r, c, num, bits;

    num = bits = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            num = (num<<1) | ((*brd)[r][c] ? 1 : 0);
            if (++bits == 4)
            {
                *buf++ = hexdigits[num];
                num = bits = 0;
            }
        }
    }
    *buf = '\0';
    assert(bits == 0);
}

void board_print(Board *brd, FILE *fp)
{
    int r, c, i;
    static const char chrs[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            i = (*brd)[r][c];
            fputc(' ', fp);
            fputc( i ==  0 ? '.' : i == -1 ? '#' : i < 0 ? '?' :
                   chrs[i%strlen(chrs)], fp );
        }
        fputc('\n', fp);
    }
}

void board_truncate(Board *brd)
{
    int n;
    Field *flds;

    flds = &((*brd)[0][0]);
    for (n = 0; n < 100; ++n) flds[n] = flds[n] == 0 ? 0 : -1;
}

bool board_is_valid_move(Board *brd, Rect *rect)
{
    int r, c;

    for (r = rect->p.r; r < rect->q.r; ++r)
    {
        for (c = rect->p.c; c < rect->q.c; ++c)
        {
            if ((*brd)[r][c] != 0) return false;
        }
    }
    return true;
}

void board_fill(Board *brd, Rect *rect, int val)
{
    int r, c;

    assert(val >= -128 && val <= 127);

    for (r = rect->p.r; r < rect->q.r; ++r)
    {
        for (c = rect->p.c; c < rect->q.c; ++c)
        {
            (*brd)[r][c] = val;
        }
    }
}
