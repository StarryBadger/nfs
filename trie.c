#include "headers.h"
#define CYN "\e[0;36m"
#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define WHITE "\x1b[37m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define ORANGE "\e[38;2;255;85;0m"
#define BMAG "\e[1;35m"
void PrintTrieLIkeAnActualTRee(struct TrieNode *root, int level)
{
    if (root == NULL)
        return;
    for (int i = 0; i < level; i++)
        printf("  ");
    if (root->isAccessible)
        printf(GREEN "%s\n" RESET, root->directory);
    else
        printf(RED "%s\n" RESET, root->directory);
    PrintTrieLIkeAnActualTRee(root->firstChild, level + 1);
    PrintTrieLIkeAnActualTRee(root->sibling, level);
}

TrieNode *createNode(char *directory, int checkFile, int checkAcc)
{
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    strcpy(newNode->directory, directory);
    newNode->firstChild = NULL;
    newNode->sibling = NULL;
    newNode->isFile = checkFile;
    newNode->isAccessible = checkAcc;
    return newNode;
}

void InsertTrie(char *directory, TrieNode *root, int checkFile, int checkAcc)
{
    // if (SearchTrie(directory, root) != NULL)
    // {
    //     return;
    // }
    // char *directory_copy = strdup(directory);
    // char *directory_copy = (char *)malloc(sizeof(char) * 1000);
    char directory_copy[100];
    strcpy(directory_copy, directory);
    char *token = strtok(directory_copy, "/");
    TrieNode *temp = root;
    while (token != NULL)
    {
        struct TrieNode *newNode = createNode(token, 0, 0);
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
    temp->isAccessible = checkAcc;
    temp->isFile = checkFile;
}

TrieNode *SearchTrie(char *directory, TrieNode *root)
{
    // char *directory_copy = strdup(directory);
    char directory_copy[100];
    strcpy(directory_copy, directory);
    char *token = strtok(directory_copy, "/");
    struct TrieNode *temp = root;
    while (token != NULL)
    {
        // printf("token : %s\n", token);
        temp = temp->firstChild;
        // printf("temp : %s\n", temp->directory);
        while (temp != NULL)
        {
            // printf("comparing %s and %s\n",temp->directory,token);
            if (strcmp(temp->directory, token) == 0 )
                break;
            temp = temp->sibling;
        }
        if (temp == NULL)
            return NULL;
        token = strtok(NULL, "/");
    }
    return temp;
}
void TrieToString(struct TrieNode *root, char *str)
{
    if (root == NULL)
        return;
    strcat(str, "(");

    if (root->isFile == 1)
        strcat(str, "F");
    else
        strcat(str, "D");

    if (root->isAccessible == 1)
        strcat(str, "A");
    else
        strcat(str, "N");

    strcat(str, root->directory);
    TrieToString(root->firstChild, str);
    strcat(str, ")");
    TrieToString(root->sibling, str);
}

TrieNode *StringToTrie(char *string)
{
    char *temp = (char *)malloc(sizeof(char) * SEND_SIZE);
    strcpy(temp, string);
    char *token;
    char *ptr_in = NULL;
    char del1[] = "()";
    int isChild = 0;
    int isInitial = 0;
    TrieNode *node_pointer;
    TrieNode *return_root;
    Stack *head = (Stack *)malloc(sizeof(Stack));
    token = __strtok_r(temp, del1, &ptr_in);
    while (token != NULL)
    {
        if (isInitial == 0)
        {
            if (string[(int)(ptr_in - temp) - 1] == '(')
            {
                isChild = 1;
                TrieNode *new;
                if (token[0] == 'F' && token[1] == 'N')
                    new = createNode(token + 2, 1, 0);
                else if (token[0] == 'F' && token[1] == 'A')
                    new = createNode(token + 2, 1, 1);
                else if (token[0] == 'D' && token[1] == 'N')
                    new = createNode(token + 2, 0, 0);
                else if (token[0] == 'D' && token[1] == 'A')
                    new = createNode(token + 2, 0, 1);

                // TrieNode *new = createNode(token);
                return_root = new;
                push(&head, new);
            }
            isInitial = 1;
        }
        else
        {
            if (isChild == 1)
            {
                if (string[(int)(ptr_in - temp) - 1] == '(')
                {
                    TrieNode *new;
                    if (token[0] == 'F' && token[1] == 'N')
                        new = createNode(token + 1, 1, 0);
                    else if (token[0] == 'F' && token[1] == 'A')
                        new = createNode(token + 2, 1, 1);
                    else if (token[0] == 'D' && token[1] == 'N')
                        new = createNode(token + 2, 0, 0);
                    else if (token[0] == 'D' && token[1] == 'A')
                        new = createNode(token + 2, 0, 1);
                    head->node->firstChild = new;
                    push(&head, new);
                }
                else
                {
                    int count = 0;
                    isChild = 0;
                    TrieNode *new;
                    if (token[0] == 'F' && token[1] == 'N')
                        new = createNode(token + 1, 1, 0);
                    else if (token[0] == 'F' && token[1] == 'A')
                        new = createNode(token + 2, 1, 1);
                    else if (token[0] == 'D' && token[1] == 'N')
                        new = createNode(token + 2, 0, 0);
                    else if (token[0] == 'D' && token[1] == 'A')
                        new = createNode(token + 2, 0, 1);
                    head->node->firstChild = new;
                    node_pointer = new;
                    while (((int)(ptr_in - temp) + (count) < strlen(string)) && string[(int)(ptr_in - temp) + (count)] == ')')
                    {
                        count++;
                        node_pointer = pop(&head);
                        ;
                    }
                }
            }
            else
            {
                if (string[(int)(ptr_in - temp) - 1] == '(')
                {
                    isChild = 1;
                    TrieNode *new;
                    if (token[0] == 'F' && token[1] == 'N')
                        new = createNode(token + 2, 1, 0);
                    else if (token[0] == 'F' && token[1] == 'A')
                        new = createNode(token + 2, 1, 1);
                    else if (token[0] == 'D' && token[1] == 'N')
                        new = createNode(token + 2, 0, 0);
                    else if (token[0] == 'D' && token[1] == 'A')
                        new = createNode(token + 2, 0, 1);
                    node_pointer->sibling = new;
                    push(&head, new);
                }
                else
                {
                    int count = 0;
                    isChild = 0;
                    TrieNode *new;
                    if (token[0] == 'F' && token[1] == 'N')
                        new = createNode(token + 2, 1, 0);
                    else if (token[0] == 'F' && token[1] == 'A')
                        new = createNode(token + 2, 1, 1);
                    else if (token[0] == 'D' && token[1] == 'N')
                        new = createNode(token + 2, 0, 0);
                    else if (token[0] == 'D' && token[1] == 'A')
                        new = createNode(token + 2, 0, 1);
                    node_pointer->sibling = new;
                    node_pointer = new;
                    while (((int)(ptr_in - temp) + (count) < strlen(string)) && string[(int)(ptr_in - temp) + (count)] == ')')
                    {
                        count++;
                        node_pointer = pop(&head);
                        ;
                    }
                }
            }
        }
        token = __strtok_r(NULL, del1, &ptr_in);
    }
    return return_root;
}
// TrieNode* SearchParentTrie

void DeleteTrie(char *directory, TrieNode *root)
{
    // char *directory_copy = strdup(directory);
    char directory_copy[100];
    strcpy(directory_copy, directory);
    char *token = strtok(directory_copy, "/");
    struct TrieNode *temp = root;
    struct TrieNode *prev = root;
    int c = 0;
    int s = 1;
    while (token != NULL)
    {
        if (s == 1)
        {
            s = 0;
            c = 1;
        }
        prev = temp;
        temp = temp->firstChild;
        while (temp != NULL)
        {

            if (strcmp(temp->directory, token) == 0)
            {
                break;
            }
            if (c == 1)
            {
                c = 0;
                s = 1;
            }
            prev = temp;
            temp = temp->sibling;
        }
        if (temp == NULL)
            return;
        token = strtok(NULL, "/");
    }
    if (c)
    {
        prev->firstChild=temp->sibling;
    }
    else if (s)
    {
        prev->sibling=temp->sibling;
    }
    free(temp);
}

char *PathParent(char *filePath)
{
    char *parentPath = (char *)malloc(sizeof(char) * 1000);
    char *lastSlash = strrchr(filePath, '/');

    if (lastSlash == NULL)
    {
        snprintf(parentPath, 1000, "%s", filePath);
    }
    else
    {
        size_t parentPathLength = lastSlash - filePath;
        snprintf(parentPath, parentPathLength + 1, "%.*s", (int)parentPathLength, filePath);
    }
    return parentPath;
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
//     TrieNode *root = NULL;

//     root = createNode("ss1", 0, 1);
//     // InsertTrie("root/dir1/dir2/dir3",root,0,0);
//     // InsertTrie("root/dir1/dir5/dir6",root,0,0);
//     // InsertTrie("root/dir1/dir2/dir4",root,0,1);
//     // InsertTrie("root/dir2/dir6/fil9",root,1,1);
//     // InsertTrie("root/dir1/dir5/dir7",root,0,1);
//     // InsertTrie("root/dir2/dir6/fil10",root,1,1);

//     InsertTrie("dir1/dir2/dir3/dir4/fil1", root, 1, 1);

//     InsertTrie("dir1/dir2/dir3/dir4", root, 0, 1);
//     InsertTrie("dir1/dir2", root, 0, 1);

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

//     //new root variable
//     printf("new root\n");
//     struct TrieNode *root2 = NULL;
//     TrieNode* newRoot = StringToTrie(str);

//     printf("New Trie:\n");
//     root2 = StringToTrie(str);
//     // PrintTrie(newRoot);
//     PrintTrieLIkeAnActualTRee(newRoot,4);

//     return 0;
// }