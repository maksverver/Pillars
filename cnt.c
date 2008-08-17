#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#define H 5
#define W 5

char buffer[1<<25];

bool open[H][W];

int fill(int r, int c)
{
    if (r < 0 || r >= H || c < 0 || c >= W || !open[r][c]) return 0;
    open[r][c] = 0;
    return fill(r + 1, c) +
           fill(r - 1, c) +
           fill(r, c + 1) +
           fill(r, c - 1) + 1;
}

bool connected(int n)
{
    int r, c;
    for (r = 0; r < H; ++r)
    {
        for (c = 0; c < W; ++c)
        {
            open[r][c] = ((n>>(r*W+c))&1);
        }
    }
    for (r = 0; r < H; ++r)
    {
        for (c = 0; c < W; ++c)
        {
            if (!open[r][c]) continue;
            return fill(r,c) == __builtin_popcount((unsigned)n);
        }
    }
    return true;
}

int main()
{
    FILE *fp;
    char path[64];
    int cnt[H*W+1][H*W+1] = { }, n, c, total;
    sprintf(path, "memo%X%X.dat", H, W);
    fp = fopen(path, "rb");
    assert(fp != NULL);
    fread(buffer, 1, 1<<(H*W), fp);
    fclose(fp);
    total = 0;
    for (n = 0; n < (1<<(H*W)); ++n)
    {
        assert(buffer[n] >= 0 && buffer[n] <= H*W);
        if (__builtin_popcount((unsigned)n) < H*W-10) continue;
        if (!connected(n)) continue;
        if (buffer[n] == __builtin_popcount((unsigned)n))
        {
            int r, c;
            for (r = 0; r < H; ++r)
            {
                for (c = 0; c < W; ++c)
                {
                    printf("%c", ((n>>(W*r+c))&1) ? '.' : '#');
                }
                printf("\n");
            }
            printf("\n");
        }
        cnt[__builtin_popcount((unsigned)n)][(int)buffer[n]] += 1;
    }

    printf("    ");
    for (c = H*W-10; c <= H*W; ++c)
    {
        printf("%6d", c);
    }
    printf("\n");

    for (n = 0; n <= H*W ; ++n)
    {
        printf("%2d: ", n);
        for (c = H*W-10; c <= H*W; ++c)
        {
            printf("%6d", cnt[c][n]);
        }
        printf("\n");
    }

    printf("    ");
    for (c = H*W-10; c <= H*W; ++c)
    {
        total = 0;
        for (n = 0; n <= H*W; ++n) total += cnt[c][n];
        printf("%6d", total);
    }
    printf("\n");

    return 0;
}
