#include "include/filter.h"

void filter(int argc, char **argv)
{
    char c[3] = {0};
    char *buff = malloc(MAX_BUFFER_SIZE);
    int i = 0;

    do
    {
        read(STDIN, (char *)&c, 2);

        if (c[0] != 0 && c[0] != 4)
        {
            putChar(c[0]);
            buff[i++] = c[0];
        }

        if (c[1] != 0 && c[1] != 4)
        {
            putChar(c[1]);
            buff[i++] = c[1];
        }

    } while (c[0] != 4 && c[1] != 4);

    buff[i] = 0;

    int j = 0;
    for (i = 0; buff[i] != 0; i++)
        if (isVowel(buff[i]))
            buff[j++] = buff[i];
    buff[j] = '\0';

    printf("\nFiltered vowels: %s\n", buff);
}

int isVowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
           c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U';
}