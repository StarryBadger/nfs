#include "headers.h"
int pathlength;
void lookFor(char *path)
{
    DIR *directory = opendir(path);
    if (directory == NULL)
    {
        fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
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
            // printf("%s\n", pathBranch + pathlength);
            printf("%s\n", pathBranch);
            lookFor(pathBranch);
        }
        else if (S_ISREG(st.st_mode))
        {
            // printf("%s\n", pathBranch + pathlength);
            printf("%s\n", pathBranch);
        }
    }
    closedir(directory);
}
void main()
{
    char path[PATH_MAX];
    scanf("%s", path);
    pathlength = strlen(path);
    lookFor(path);
}