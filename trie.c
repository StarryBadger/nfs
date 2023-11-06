#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

typedef struct TrieNode
{
    char directory[100];
    struct TrieNode *firstChild;
    struct TrieNode *sibling;
    int isFile;
}TrieNode;

TrieNode *root = NULL;

TrieNode *createNode(char *directory)
{
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    strcpy(newNode->directory, directory);
    newNode->firstChild = NULL;
    newNode->sibling = NULL;
    newNode->isFile = 0;
    return newNode;
}

void InsertTrie(char *directory)
{
    char *directory_copy = strdup(directory); 
    char *token = strtok(directory_copy, "/");
    TrieNode *temp = root;
    while (token != NULL)
    {
        struct TrieNode *newNode = createNode(token);
        if (temp->firstChild == NULL)
        {
            temp->firstChild = newNode;
            temp = temp->firstChild;
        }
        else
        {
            temp = temp->firstChild;
            while (temp->sibling != NULL)
            {
                if (strcmp(temp->directory, token) == 0)
                    break;
                temp = temp->sibling;
            }
            if (strcmp(temp->directory, token) != 0)
            {
                temp->sibling = newNode;
                temp = temp->sibling;
            }
        }
        token = strtok(NULL, "/");
    }
    temp->isFile = 1;
} 

TrieNode* SearchTrie(char *directory)
{
    char *directory_copy = strdup(directory); 
    char *token = strtok(directory_copy, "/");
    struct TrieNode *temp = root;
    while (token != NULL)
    {
        temp = temp->firstChild;
        while (temp != NULL)
        {
            if (strcmp(temp->directory, token) == 0)
                break;
            temp = temp->sibling;
        }
        if (temp == NULL)
            return NULL;
        token = strtok(NULL, "/");
    }
    return temp;
}

void DeleteTrie(char *directory)
{
    char *directory_copy = strdup(directory); 
    char *token = strtok(directory_copy, "/");
    struct TrieNode *temp = root;
    while (token != NULL)
    {
        temp = temp->firstChild;
        while (temp != NULL)
        {
            if (strcmp(temp->directory, token) == 0)
                break;
            temp = temp->sibling;
        }
        if (temp == NULL)
            return;
        token = strtok(NULL, "/");
    }
    free(temp);
}

void PrintTrie(struct TrieNode *root)
{
    if (root == NULL)
        return;
    printf("%s\n", root->directory);
    PrintTrie(root->firstChild);
    PrintTrie(root->sibling);
}

// int main()
// {

//     root = createNode("storage_server_1");
//     InsertTrie("root/dir1/dir2/dir3");
//     // InsertTrie("root/dir1/dir5/dir6");
//     // InsertTrie("root/dir1/dir2/dir4");
//     // InsertTrie("root/dir1/dir5/dir7");
//     // InsertTrie("root/dir2/dir6/dir9");
//     // InsertTrie("root/dir2/dir6/dir10");
//     PrintTrie(root);

//     SearchTrie("root/dir1/dir2/dir3") == NULL ? printf("Not Found\n") : printf("Found\n");
//     DeleteTrie("root/dir1/dir2/dir3");
//     SearchTrie("root/dir1/dir2/dir3") == NULL ? printf("Not Found\n") : printf("Found\n");
//     return 0;
// }