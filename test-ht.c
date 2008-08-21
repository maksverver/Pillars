#include "HashTable.c"
#include <stdio.h>
#include <string.h>

HashTable *ht;

int main()
{
    char line[1024], *key;
    ht = HT_create(1024, 5);

    for (;;)
    {
        if (!fgets(line, 1024, stdin)) break;
        if (line[0] == '?')
        {
            key = malloc(1024);
            memset(key, 0, 1024);
            strcpy(key, line + 1);
            if (HT_get(ht, key)) {
                printf("found\n");
            } else {
                printf("not found\n");
            }
        }
        else
        {
            key = malloc(1024);
            memset(key, 0, 1024);
            strcpy(key, line);
            if (HT_set(ht, key, key, NULL, NULL)) {
                printf("replaced\n");
            } else {
                printf("added\n");
            }
        }
        
    }
    return 0;
}
