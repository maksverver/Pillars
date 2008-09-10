#include <Board.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void print_cdata(const char *msg)
{
    while (*msg)
    {
        int i = *(unsigned char*)msg++;
        if (i < 32 || i >= 127)
        {
            printf("&#%d;", i);
        }
        else
        {
            switch (i)
            {
            case '<':  printf("&lt;"); break;
            case '>':  printf("&gt;"); break;
            case '&':  printf("&amp;"); break;
            case '"':  printf("&quot;"); break;
            case '\'': printf("&apos;"); break;
            default:   putchar(i);
            }
        }
    }
}

static int perm_to_num(int *perm)
{
    int av[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int res, n, m;

    res = 0;
    for (n = 0; n < 10; ++n)
    {
        for (m = 0; m < 10 - n; ++m) if (av[m] == perm[n]) break;
        res = res*(10 - n) + m;
        for (; m + 1 < 10 - n; ++m) av[m] = av[m + 1];
    }
    return res;
}

static void print_pillars(Board *b)
{
    int r, c, n;
    int perm[10];
    bool valid_perm;
    bool used[10];

    for (n = 0; n < 10; ++n) perm[n] = -1;
    for (n = 0; n < 10; ++n) used[n] = false;
    valid_perm = true;

    /* Find pillars & normalize board */
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] < 0)
            {
                (*b)[r][c] = -1;
                if (used[c] || perm[r] != -1)
                {
                    valid_perm = false;
                }
                else
                {
                    used[c] = false;
                    perm[r] = c;
                }
            }
        }
    }

    for (n = 0; n < 10; ++n) if (perm[n] == -1) valid_perm = false;

    if (valid_perm)
    {
        printf("<pillars perm='%d'>", perm_to_num(perm) + 1);
    }
    else
    {
        printf("<pillars>");
    }

    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] < 0)
            {
                printf("<point>%c%c</point>", "ABCDEFGHIJ"[r], "abcdefghij"[c]);
            }
        }
    }

    printf("</pillars>");
}

static void print_moves(Board *b)
{
    int num_moves, n, r, c;
    Rect move;

    num_moves = 0;
    while (board_get_move(b, NULL, num_moves)) ++num_moves;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] > num_moves) (*b)[r][c] = 0;
        }
    }

    printf("<moves count='%d'>", num_moves);
    for (n = 0; n < num_moves; ++n)
    {
        board_get_move(b, &move, n);
        printf("<rect>%c%c%c%c</rect>",
            "ABCDEFGHIJ"[move.p.r], "abcdefghij"[move.p.c],
            "ABCDEFGHIJ"[move.q.r - 1], "abcdefghij"[move.q.c - 1]);
    }
    printf("</moves>");
}

static void print_board(Board *b)
{
    char buf[201];

    board_encode_full(b, buf);
    printf("<board>%s</board>", buf);
}

static void print_result(Board *b)
{
    int open, turn, r, c;

    turn = 0;
    open = 0;
    for (r = 0; r < 10; ++r)
    {
        for (c = 0; c < 10; ++c)
        {
            if ((*b)[r][c] == 0) ++open;
            if ((*b)[r][c] > turn) turn = (*b)[r][c];
        }
    }

    if (open == 0)
    {
        printf("<result winner='%d' score1='%d' score2='%d'>"
               "Player %d won.</result>",
                1 + (turn%2),
                (turn%2) == 0 ? 18 : 9,
                (turn%2) == 0 ?  9 : 18,
                1 + (turn%2) );
    }
}

int main(int argc, char *argv[])
{
    Board b;
    const char *desc, *player1, *player2;

    if (argc != 2 && argc != 4)
    {
        fprintf(stderr, "Usage: board-to-xml <board> [<player1> <player2>]\n");
        exit(1);
    }
    desc    = argv[1];
    player1 = argc > 2 ? argv[2] : "Red";
    player2 = argc > 3 ? argv[3] : "Blue";

    if (!board_decode_full(&b, argv[1]) && !board_decode_short(&b, argv[1]))
    {
        fprintf(stderr, "Unable to decode board definition!\n");
        exit(1);
    }

    printf("<?xml version='1.0' encoding='UTF-8'?>");
    printf("<game>");

    if (player1 != NULL)
    {
        printf("<player1><name>");
        print_cdata(player1);
        printf("</name></player1>");
    }

    if (player2 != NULL)
    {
        printf("<player2><name>");
        print_cdata(player2);
        printf("</name></player2>");
    }

    print_pillars(&b);

    print_moves(&b);

    print_board(&b);

    print_result(&b);

    printf("</game>");

    return 0;
}
