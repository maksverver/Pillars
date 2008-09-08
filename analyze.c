#include "Analysis.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* Given a GroupInfo (complete with nim-values) determines if the current
   position is known to be losing. If any nim-values are not known, false is
   returned. */
static bool is_losing(GroupInfo *gi)
{
    int n, num_ones, nsum;

    num_ones = 0;
    nsum = 0;
    for (n = 0; n < gi->num_groups; ++n)
    {
        if (gi->size[n] == 1) ++num_ones;
        if (gi->nval[n] < 0) return false;   /* value unknown! */
        nsum ^= gi->nval[n];
    }

    return gi->num_groups == num_ones ? nsum != 0 : nsum == 0;
}

int main(int argc, char *argv[])
{
    Board b;
    GroupInfo gi;
    int n, nsum, num_ones;
    char buf[256];

    analysis_initialize();

    if (argc != 2)
    {
        fprintf(stderr, "Expected 1 argument: board definition.\n");
        exit(1);
    }

    if (!board_decode_full(&b, argv[1]) && !board_decode_short(&b, argv[1]))
    {
        fprintf(stderr, "Unable to decode board definition!\n");
        exit(1);
    }

    printf("Board given:\n");
    board_print(&b, stdout);

    board_encode_full(&b, buf);
    printf("Full board decription:\n%.50s\n%.50s\n%.50s\n%.50s\n",
           buf, buf + 50, buf + 100, buf + 150 );
    board_encode_short(&b, buf);
    printf("Short board description: %s\n", buf);

    printf("\nAnalysing groups...\n");
    board_flatten(&b);
    analysis_identify_groups(&b, &gi);
    board_print(&b, stdout);
    printf("%d groups identified\n", gi.num_groups);

    analysis_nim_values(&b, &gi);
    nsum = 0;
    num_ones = 0;
    for (n = 0; n < gi.num_groups; ++n)
    {
        num_ones += (gi.nval[n] == 1);
        if (gi.nval[n] < 0) nsum = -1;
        if (nsum != -1) nsum ^= gi.nval[n];

        printf("%2d: nv=%2d (%2d,%2d)-(%2d,%2d) [%dx%d]  (%d,%d) %d fields\n",
            n, gi.nval[n],
            gi.bounds[n].p.r, gi.bounds[n].p.c,
            gi.bounds[n].q.r, gi.bounds[n].q.c,
            gi.bounds[n].q.r - gi.bounds[n].p.r,
            gi.bounds[n].q.c - gi.bounds[n].p.c,
            gi.first[n].r, gi.first[n].c, gi.size[n] );

        /*
        {
            FILE *fp;
            int m;
            fp = fopen("test.dat","wb");
            for (m = (1<<gi.size[n])-1; m >= 0; --m)
            {
                fputc(memo[n], fp);
            }
            assert(fp != NULL);
            fclose(fp);
        }
        */
    }

    if (nsum < 0)
    {
        printf("Game status unknown!\n");
    }
    else
    if (num_ones == gi.num_groups)
    {
        printf( "Player %s (any move)\n",
                (num_ones%2 == 0) ? "wins" : "loses" );
    }
    else
    if (num_ones == gi.num_groups - 1)
    {
        printf("One group with nim value <> 1 left; player wins.\n");
    }
    else
    if (nsum == 0)
    {
        printf("Nim sum 0; player loses (any move).\n");
    }
    else
    {
        printf("Nim sum %d; player wins.\n", nsum);
    }

    /* Search for winning moves */
    {
        Rect m;
        board_clear(&b);
        for (m.p.r = 0; m.p.r < 10; ++m.p.r)
        {
            for (m.p.c = 0; m.p.c < 10; ++m.p.c)
            {
                for (m.q.r = m.p.r + 1; m.q.r <= 10; ++m.q.r)
                {
                    for (m.q.c = m.p.c + 1; m.q.c <= 10; ++m.q.c)
                    {
                        if (board_is_valid_move(&b, &m))
                        {
                            GroupInfo gi2;
                            board_fill(&b, &m, -1);
                            analysis_identify_groups(&b, &gi2);
                            analysis_nim_values(&b, &gi2);
                            board_clear(&b);
                            if (is_losing(&gi2))
                            {
                                char buf[64];
                                rect_encode(&m, buf);
                                board_encode_short(&b, buf + 5);
                                printf("Move to make nsum 0: %s (%s)\n",
                                       buf, buf + 5);
                            }
                            board_fill(&b, &m, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
