#include "headers.h"


TrieNode *createNode(char *directory)
{
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    strcpy(newNode->directory, directory);
    newNode->firstChild = NULL;
    newNode->sibling = NULL;
    newNode->isFile = 0;
    return newNode;
}

void InsertTrie(char *directory,TrieNode* root)
{
    if(SearchTrie(directory,root)!=NULL)
    {
        return;
    }
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

TrieNode* SearchTrie(char *directory,TrieNode* root)
{
    char *directory_copy = strdup(directory); 
    char *token = strtok(directory_copy, "/");
    struct TrieNode *temp = root;
    while (token != NULL)
    {
        printf("token : %s\n",token);
        temp = temp->firstChild;
        printf("temp : %s\n",temp->directory);
        while (temp != NULL)
        {
            // printf("comparing %s and %s\n",temp->directory,token);
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

// TrieNode* SearchParentTrie

void DeleteTrie(char *directory,TrieNode* root)
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



void TrieToString(struct TrieNode *root, char *str)
{
    if (root == NULL)
        return;
    strcat(str, "(");
    strcat(str, root->directory);
    TrieToString(root->firstChild, str);
    strcat(str, ")");
    TrieToString(root->sibling, str);
}

TrieNode* StringToTrie(char *str) {
    TrieNode* newRoot = NULL;
    TrieNode* current = NULL;
    char* token = strtok(str, "()");
    
    while (token != NULL) {
        if (newRoot == NULL) {
            newRoot = createNode(token);
            current = newRoot;
        } else if (strcmp(token, "(") == 0) {
            // Create a child node
            token = strtok(NULL, "()");
            TrieNode* newNode = createNode(token);
            if (current->firstChild == NULL) {
                current->firstChild = newNode;
                current = current->firstChild;
            } else {
                current = current->firstChild;
                while (current->sibling != NULL) {
                    current = current->sibling;
                }
                current->sibling = newNode;
                current = current->sibling;
            }
        } else if (strcmp(token, ")") == 0) {
            current = newRoot;
        } else {
            TrieNode* newNode = createNode(token);
            current->sibling = newNode;
            current = current->sibling;
        }
        token = strtok(NULL, "()");
    }

    return newRoot;
}

// TrieNode* StringToTrie(char* str)
// {
//     TrieNode* newRoot = NULL;
//     TrieNode* current = NULL;
//     // char* token = strtok_r(str, "()", &str);
    
// }

// int main()
// {
//     TrieNode *root = NULL;

//     root = createNode("ss1");
//     InsertTrie("root/dir1/dir2/dir3",root);
//     InsertTrie("root/dir1/dir5/dir6",root);
//     InsertTrie("root/dir1/dir2/dir4",root);
//     InsertTrie("root/dir1/dir5/dir7",root);
//     InsertTrie("root/dir2/dir6/dir9",root);
//     InsertTrie("root/dir2/dir6/dir10",root);

//     // InsertTrie("a/b/e", root);
//     // InsertTrie("a/c");
//     // InsertTrie("a/d/f");
//     // InsertTrie("a/d/g");
//     PrintTrie(root);

//     // SearchTrie("root/dir1/dir2/dir3",root) == NULL ? printf("Not Found\n") : printf("Found\n");
//     // DeleteTrie("root/dir1/dir2/dir3",root);
//     // SearchTrie("root/dir1/dir2/dir3",root) == NULL ? printf("Not Found\n") : printf("Found\n");

//     char str[1000] = "";
//     TrieToString(root, str);
//     printf("%s\n", str);

//     // //new root variable
//     // printf("new root\n");
//     struct TrieNode *root2 = NULL;
//     TrieNode* newRoot = StringToTrie(str);

//     printf("New Trie:\n");
//     root2 = StringToTrie(str);
//     PrintTrie(newRoot);


//     return 0;
// }