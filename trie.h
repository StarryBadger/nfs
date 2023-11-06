#ifndef TRIE_H
typedef struct TrieNode
{
    char directory[100];
    struct TrieNode *firstChild;
    struct TrieNode *sibling;
    int isFile;
}TrieNode;

TrieNode *createNode(char *directory);

void InsertTrie(char *directory);

TrieNode* SearchTrie(char *directory);

void DeleteTrie(char *directory);

void PrintTrie(struct TrieNode *root);

#endif