#include "headers.h"
void appendCharToFront(char *str, char character)
{
    int length = strlen(str);
    for (int i = length; i >= 0; i--)
    {
        str[i + 1] = str[i];
    }
    str[0] = character;
}
void lookFor(char *path, int pathlength, TrieNode *root)
{
    DIR *directory = opendir(path);
    char TEMP_BUFF[PATH_MAX+1];
    if (directory == NULL)
    {
        fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        // hidden files should not be consdiered
        if (entry->d_name[0] == '.' || (strlen(entry->d_name) > 1 && entry->d_name[1] == '.'))
        {
            continue;
        }
        char pathBranch[PATH_MAX];
        strcpy(pathBranch, path);
        if (pathBranch[strlen(pathBranch) - 1] != '/')
            strcat(pathBranch, "/");
        strcat(pathBranch, entry->d_name);
        struct stat st;
        if (stat(pathBranch, &st) == -1)
            continue;
        if (S_ISDIR(st.st_mode))
        {
            printf("%s\n", pathBranch + pathlength);
            strcpy(TEMP_BUFF,pathBranch + pathlength);
            appendCharToFront(TEMP_BUFF,'D');
            InsertTrie(TEMP_BUFF, root);
            lookFor(pathBranch, pathlength, root);
        }
        else if (S_ISREG(st.st_mode))
        {
            printf("%s\n", pathBranch + pathlength);
            strcpy(TEMP_BUFF,pathBranch + pathlength);
            appendCharToFront(TEMP_BUFF,'F');
            InsertTrie(TEMP_BUFF, root);
        }
    }
    closedir(directory);
}
// void seek()
// {
//     char path[PATH_MAX];
//     scanf("%s", path);
//     int pathlength = strlen(path);
//     lookFor(path,pathlength,ssTrie);
// }