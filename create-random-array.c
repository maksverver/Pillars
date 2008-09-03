#include <stdio.h>

/* Writes 100 random 32-bit values in C source code format */

int main()
{
    int n;
    unsigned v;
    FILE *fp;

    fp = fopen("/dev/random", "rb");
    for (n = 0; n < 100; ++n)
    {
        fread(&v, sizeof(v), 1, fp);
        printf("0x%08xu,%c", v, n%5==4 ?'\n':' ');
    }
    fclose(fp);
    return 0;
}
