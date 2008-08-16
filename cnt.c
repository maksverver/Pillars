#include <stdio.h>
#include <assert.h>

char buffer[1<<25];
int main()
{
    FILE *fp;
    int cnt[26] = { }, n;
    fp = fopen("memo55.dat", "rb");
    assert(fp != NULL);
    fread(buffer, 1, 1<<25, fp);
    fclose(fp);
    for (n = 0; n < (1<<25); ++n)
    {
        if (buffer[n] == -1)
        {
            int r, c;
            for (r = 0; r < 5; ++r)
            {
                for (c = 0; c < 5; ++c)
                {
                    printf("%c", ((n>>(5*r+c))&1) ? '.' : '#');
                }
                printf("\n");
            }
            printf("\n");
        }
        assert(buffer[n] >= 0 && buffer[n] < 26);
        cnt[(int)buffer[n]] += 1;
    }
    for (n = 0; n < 26; ++n)
    {
        printf("%8d\n", cnt[n]);
    }
    return 0;
}
