#include "headers.h"
int foundcount = 0;
int pathlength;
char executeMe[PATH_MAX];
int setSeekFlags(char *input, bool *F, bool *E, bool *D)
{
    if (equal(input, "-e") ||
        equal(input, "-d") ||
        equal(input, "-f") ||
        equal(input, "-ef") ||
        equal(input, "-fe") ||
        equal(input, "-ed") ||
        equal(input, "-de"))
    {
        for (int i = 0; i < length(input); i++)
        {
            if (input[i] == 'f')
            {
                *F = true;
            }
            else if (input[i] == 'd')
            {
                *D = true;
            }
            else if (input[i] == 'e')
            {
                *E = true;
            }
        }
        return 1;
    }
    else if (equal(input, "-fd") ||
             equal(input, "-df") ||
             equal(input, "-def") ||
             equal(input, "-dfe") ||
             equal(input, "-edf") ||
             equal(input, "-efd") ||
             equal(input, "-fde") ||
             equal(input, "-fed"))
    {
        return 2;
    }
    else
    {
        return 0;
    }
}
void lookFor(bool D, bool F, char *target, char *path)
{
    DIR *directory = opendir(path);
    if (directory == NULL)
    {
        fprintf(stderr, "\x1b[31mseek: Could not open %s. Permission denied\n\n\x1b[0m", path);
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL)
    {
        if (equal(entry->d_name, ".") || equal(entry->d_name, ".."))
        {
            continue;
        }
        char pathBranch[PATH_MAX];
        mystrcpy(pathBranch, path);
        if (pathBranch[length(pathBranch) - 1] != '/')
            strcat(pathBranch, "/");
        strcat(pathBranch, entry->d_name);
        struct stat st;
        if (stat(pathBranch, &st) == -1)
            continue;
        if (S_ISDIR(st.st_mode))
        {
            if (equal(entry->d_name, target) && !F)
            {
                printf(BLUE ".%s\n" RESET, pathBranch + pathlength);
                foundcount++;
                mystrcpy(executeMe, pathBranch);
            }
            lookFor(D, F, target, pathBranch);
        }
        else if (S_ISREG(st.st_mode) && !D)
        {
            if (equal(entry->d_name, target) || equalNameWithExtension(entry->d_name, target))
            {
                printf(GREEN ".%s\n" RESET, pathBranch + pathlength);
                foundcount++;
                mystrcpy(executeMe, pathBranch);
            }
        }
    }
    closedir(directory);
}
void seek(command cmd)
{
    foundcount = 0;
    bool F = false, E = false, D = false;
    bool targetset = false;
    char target[PATH_MAX], path[PATH_MAX];
    int i = 1;
    int res;
    if (cmd.argc == 1)
    {
        fprintf(stderr, "\x1b[31mseek: No target file/directory\n\x1b[0m");
        return;
    }
    for (int i = 1; i < cmd.argc; i++)
    {
        if (res = setSeekFlags(cmd.argv[i], &F, &E, &D))
        {
            if (res == 2)
            {
                fprintf(stderr, "\x1b[31mseek: Invalid flags\n\x1b[0m");
                return;
            }
            continue;
        }
        strcpy(target, cmd.argv[i]);
        targetset = true;
        if (i == cmd.argc - 1)
        {
            getcwd(path, PATH_MAX - 1);
            break;
        }
        else
        {
            mystrcpy(path, cmd.argv[i + 1]);
            break;
        }
        if (i + 1 != cmd.argc - 1)
        {
            fprintf(stderr, "\x1b[31mseek: Multiple file paths detected\n\x1b[0m");
            return;
        }
    }
    if (!targetset)
    {
        fprintf(stderr, "\x1b[31mseek: No target file/directory\n\x1b[0m");
        return;
    }
    if (F && D)
    {
        fprintf(stderr, "\x1b[31mseek: Invalid flags\n\x1b[0m");
        return;
    }
    if (prefix("~", path))
    {
        mystrcpy(path, replaceTildeWithHome(path));
    }
    pathlength = length(path);
    lookFor(D, F, target, path);
    if (foundcount == 0)
    {
        printf("No match found!\n");
    }
    if (E)
    {
        if (foundcount == 1)
        {
            struct stat st;
            stat(executeMe, &st);
            if (S_ISDIR(st.st_mode))
            {
                chdir(executeMe);
            }
            else if (S_ISREG(st.st_mode))
            {
                FILE *file = fopen(executeMe, "r");
                if (file == NULL)
                {
                    fprintf(stderr, "\x1b[31mseek: Error in opening file\n\x1b[0m");
                    return;
                }
                char ch;
                while ((ch = fgetc(file)) != EOF)
                    putchar(ch);
                fclose(file);
            }
        }
        else if (foundcount > 2)
        {
            fprintf(stderr, "\x1b[31mseek: can't execute more than one file/directory\n\x1b[0m");
            return;
        }
    }
}