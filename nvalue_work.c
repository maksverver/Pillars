#include "Analysis.h"

/* Does the real work for nvalue */
int nvalue_work(int grp_size)
{
    int n, nval;
    Mask mask, move;
    uint32_t nvals;

    mask = ((Mask)1<<grp_size);
    do {
        --mask;

        nvals = 0;
        n = 0;
        while ((move = moves[n]) != 0)
        {
            if (mask&move)
            {
                n += skip[n];
            }
            else
            {
                nvals |= (1<<memo[mask|move]);
                n += 1;
            }
        }

        nval = 0;
        while ((nvals & (1 << nval)) != 0) ++nval;
        memo[mask] = nval;
    } while (mask > 0);

    return memo[0];
}
