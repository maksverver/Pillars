#include <stdint.h>

typedef uint32_t Mask;
extern char memo[];
extern Mask moves[];
extern int skip[];

/* Does the real work for nvalue_new */
int nvalue_new_work(int grp_size)
{
    int result, n;
    Mask mask, move;
    uint32_t nvals;

    result = -1;

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
        result = 0;
        while ((nvals & (1 << result)) != 0) ++result;
        memo[mask] = result;
    } while(mask > 0);

    return result;
}
