#include "Board.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static const char hexdigits[]    = "0123456789ABCDEF";
static const char base32digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

static bool is_hex_string(const char *str)
{
    while (*str)
    {
        if (strchr(hexdigits, *str) == NULL) return false;
        str += 1;
    }
    return true;
}

static bool is_base32_string(const char *str)
{
    while (*str)
    {
        if (strchr(base32digits, *str) == NULL) return false;
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
    if (strlen(str) != 20 || !is_base32_string(str)) return false;

    r = c = 0;
    while (*str)
    {
        i = strchr(base32digits, *str++) - base32digits;
        for (n = 4; n >= 0; --n)
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
            if (++bits == 5)
            {
                *buf++ = base32digits[num];
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

    fputs("  abcdefghij\n", fp);
    for (r = 0; r < 10; ++r)
    {
        fputc('A' + r, fp);
        fputc(' ', fp);
        for (c = 0; c < 10; ++c)
        {
            i = (*brd)[r][c];
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

void board_clear(Board *brd)
{
    int n;
    Field *flds;

    flds = &((*brd)[0][0]);
    for (n = 0; n < 100; ++n) flds[n] = flds[n] >= 0 ? 0 : -1;
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

int board_empty_area(Board *brd)
{
    int r, c, res;

    res = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            res += (*brd)[r][c] == 0;
        }
    }
    return res;
}

static void permute10(int *arr, int number)
{
    static int fac[10] = { 1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880 };
    int pos, i, k, tmp;

    for (pos = 0; pos < 9; ++pos)
    {
        assert(number < fac[10 - pos]);

        k = pos + number/fac[10 - pos - 1];
        tmp = arr[k];
        for (i = k; i > pos; --i) arr[i] = arr[i - 1];
        arr[pos] = tmp;

        number %= fac[10 - pos - 1];
    }
}

void board_construct(Board *brd, int number)
{
    int nums[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int r, c;

    assert(number >= 0 && number < NUM_BOARDS);
    permute10(nums, number);
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            (*brd)[r][c] = 0;
        }
        (*brd)[r][nums[r]] = -1;
    }
}

bool point_decode(Point *p, const char *buf)
{
    int r, c;

    if (buf[0] == '\0' || buf[1] == '\0') return false;

    r = buf[0] - 'A';
    c = buf[1] - 'a';
    if (r < 0 || r >= 10 || c < 0 || c >= 10) return false;

    p->r = r;
    p->c = c;
    return true;
}

void point_encode(const Point *p, char buf[3])
{
    assert(p->r >= 0 && p->r < 10);
    assert(p->c >= 0 && p->c < 10);
    buf[0] = 'A' + p->r;
    buf[1] = 'a' + p->c;
    buf[2] = '\0';
}

bool rect_decode(Rect *r, const char *buf)
{
    Point p, q;

    if (!point_decode(&p, buf) || !point_decode(&q, buf + 2)) return false;
    if (q.r < p.r || q.c < p.c) return false;
    r->p = p;
    r->q = q;
    r->q.r += 1;
    r->q.c += 1;
    return r;
}

void rect_encode(const Rect *r, char buf[5])
{
    assert(r->p.r >= 0 && r->p.r < 10);
    assert(r->p.c >= 0 && r->p.c < 10);
    assert(r->q.r > r->p.r && r->q.r <= 10);
    assert(r->q.c > r->p.c && r->q.c <= 10);
    buf[0] = 'A' + r->p.r;
    buf[1] = 'a' + r->p.c;
    buf[2] = 'A' + r->q.r - 1;
    buf[3] = 'a' + r->q.c - 1;
    buf[4] = '\0';
}

int board_list_moves(Board *brd, Rect *moves)
{
    /* NOTE: this can be optimized some more, if necessary */

    Rect r;
    int cnt;

    cnt = 0;
    for (r.p.r = 0; r.p.r < 10; ++r.p.r)
    {
        for (r.p.c = 0; r.p.c < 10; ++r.p.c)
        {
            if ((*brd)[r.p.r][r.p.c] != 0) continue;
            for (r.q.r = r.p.r + 1; r.q.r <= 10; ++r.q.r)
            {
                for (r.q.c = r.p.c + 1; r.q.c <= 10; ++r.q.c)
                {
                    if (!board_is_valid_move(brd, &r)) break;
                    if (moves) *moves++ = r;
                    ++cnt;
                }
            }
        }
    }
    return cnt;
}

bool board_get_move(Board *brd, Rect *move, int n)
{
    int r, c, cnt;
    Rect m;

    m.p.r = m.p.c = 10;
    m.q.r = m.q.c = 0;
    cnt = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*brd)[r][c] == n)
            {
                if (r < m.p.r) m.p.r = r;
                if (c < m.p.c) m.p.c = c;
                if (r >= m.q.r) m.q.r = r + 1;
                if (c >= m.q.c) m.q.c = c + 1;
                cnt += 1;
            }
        }
    }

    if (cnt == 0 || cnt != (m.q.r - m.p.r)*(m.q.c - m.p.c)) return false;

    *move = m;
    return true;
}
