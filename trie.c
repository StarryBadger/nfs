#include "headers.h"

void PrintTrieLIkeAnActualTRee(struct TrieNode *root, int level)
{
    if (root == NULL)
        return;
    for (int i = 0; i < level; i++)
        printf("  ");
    printf("%s\n", root->directory);
    PrintTrieLIkeAnActualTRee(root->firstChild, level + 1);
    PrintTrieLIkeAnActualTRee(root->sibling, level);
}

TrieNode *createNode(char *directory)
{
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    strcpy(newNode->directory, directory);
    newNode->firstChild = NULL;
    newNode->sibling = NULL;
    newNode->isFile = 0;
    return newNode;
}

void InsertTrie(char *directory, TrieNode *root)
{
    if (SearchTrie(directory, root) != NULL)
    {
        return;
    }
    // char *directory_copy = strdup(directory);
    // char *directory_copy = (char *)malloc(sizeof(char) * 1000); 
    char directory_copy[100];
    strcpy(directory_copy, directory);
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

TrieNode *SearchTrie(char *directory, TrieNode *root)
{
    // char *directory_copy = strdup(directory);
    char directory_copy[100];
    strcpy(directory_copy,directory);
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

TrieNode *StringToTrie(char *string)
{
    char *temp = (char *)malloc(sizeof(char) * 500);
    strcpy(temp, string);
    char *token;
    char *ptr_in = NULL;
    char del1[] = "()";
    int isChild = 0;
    int isInitial=0;
    TrieNode *node_pointer;
    TrieNode* return_root;
    Stack *head = (Stack *)malloc(sizeof(Stack));
    token = __strtok_r(temp, del1, &ptr_in);
    while (token != NULL)
    {
        if (isInitial == 0)
        {
            if (string[(int)(ptr_in - temp) - 1] == '(')
            {
                isChild = 1;
                TrieNode *new = createNode(token);
                return_root=new;
                push(&head, new);
            }
            isInitial=1;
        }
        else{
            if(isChild==1)
            {
                if(string[(int)(ptr_in-temp)-1]=='(')
                {
                    TrieNode* new=createNode(token);
                    head->node->firstChild=new;
                    push(&head,new);
                }
                else{
                    int count=0;
                    isChild=0;
                    TrieNode* new=createNode(token);
                    head->node->firstChild=new;
                    node_pointer=new;
                    while(((int)(ptr_in-temp)+(count) < strlen(string))  && string[(int)(ptr_in-temp)+(count)]==')')
                    {
                        count++;
                        node_pointer=pop(&head);;  
                    }
                }
            }
            else{
                if(string[(int)(ptr_in-temp)-1]=='(')
                {
                    isChild=1;
                    TrieNode* new=createNode(token);
                    node_pointer->sibling=new;
                    push(&head,new);
                }
                else{
                    int count=0;
                    isChild=0;
                    TrieNode*new=createNode(token);
                    node_pointer->sibling=new;
                    node_pointer=new;
                    while(((int)(ptr_in-temp)+(count) < strlen(string))  && string[(int)(ptr_in-temp)+(count)]==')')
                    {
                        count++;
                        node_pointer=pop(&head);;  
                    }
                }
            }
        }
        token=__strtok_r(NULL,del1,&ptr_in);
    }
    return return_root;
}
// TrieNode* SearchParentTrie

void DeleteTrie(char *directory, TrieNode *root)
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

char* PathParent(char* filePath) 
{
    char* parentPath = (char*)malloc(sizeof(char) * 1000);
    char* lastSlash = strrchr(filePath, '/');

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
//     // PrintTrie(newRoot);
//     PrintTrieLIkeAnActualTRee(newRoot,4);

//     return 0;
// }