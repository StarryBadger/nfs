#include <stdio.h>
#include <string.h>

int main()
{
    char str[100] = "/hi/hello/hey/hu/hoi";
    char character = 'X';
    char modified_string[100];
    int lastIndexOfSlash;
    strcpy(modified_string, str);

    int i, j;
    j = 0;

    for (i = 0; i < strlen(modified_string); i++)
    {
        str[j++] = modified_string[i];

        if (modified_string[i] == '/')
        {
            str[j] = 'D';
            lastIndexOfSlash = j;
            j++;
        }
    }
    str[lastIndexOfSlash]=character;
    str[j] = '\0'; // Null-terminate the modified string

    printf("%s\n", str);

    return 0;
}
