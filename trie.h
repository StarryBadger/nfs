#ifndef __TRIE_H
#define __TRIE_H

typedef struct TrieNode
{
    char directory[100];
    struct TrieNode *firstChild;
    struct TrieNode *sibling;
    int isFile;
}TrieNode;

typedef struct Stack
{
    TrieNode *node;
    struct Stack *next;
} Stack;

void push(Stack **head, TrieNode *node);

TrieNode *pop(Stack **head);

int isEmpty(Stack *head);

TrieNode *createNode(char *directory);

void InsertTrie(char *directory,TrieNode* root);

TrieNode* SearchTrie(char *directory,TrieNode* root);

void DeleteTrie(char *directory,TrieNode* root);

void PrintTrie(struct TrieNode *root);

void TrieToString(struct TrieNode *root, char *str);

TrieNode* StringToTrie(char *str);

char* PathParent(char* path);

#endif