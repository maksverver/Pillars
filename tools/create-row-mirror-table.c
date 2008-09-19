/* Generates tables to mirror a bitstring of a given size (between 1 and 10,
   inclusive) represented as an integer. */

#include <stdio.h>

int main()
{
    int w = 0, n, x, y;
    for (w = 1; w <= 10; ++w)
    {
        printf("static const uint16_t mirror%d[%d] = {", w, (1<<w));
        for (x = 0; x < (1<<w); ++x)
        {
            y = 0;
            for (n = 0; n < w; ++n) y = (y<<1)|((x>>n)&1);
            if (x > 0) printf(",");
            if (x%12 == 0) printf("\n");
            printf("%5d", y);
        }
        printf(" };\n");
    }
    return 0;
}
