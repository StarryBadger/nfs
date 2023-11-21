#ifndef __TRIE_H
#define __TRIE_H

#include <pthread.h>

typedef struct TrieNode
{
    char directory[100];
    struct TrieNode *firstChild;
    struct TrieNode *sibling;
    int isFile;
    int isAccessible;
    // pthread_rw_lock_t rwlock; 
    // sem_t file_lock;
}TrieNode;

void PrintTrieLIkeAnActualTRee(struct TrieNode *root, int level);

TrieNode *createNode(char *directory, int checkFile, int checkAcc);

void InsertTrie(char *directory,TrieNode* root, int checkFile, int checkAcc);

TrieNode* SearchTrie(char *directory,TrieNode* root);

void DeleteTrie(char *directory,TrieNode* root);

void PrintTrie(struct TrieNode *root);

void TrieToString(struct TrieNode *root, char *str);

TrieNode* StringToTrie(char *str);

char* PathParent(char* path);

#endif