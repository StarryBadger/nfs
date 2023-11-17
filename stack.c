#include "headers.h"

void push(Stack **head, TrieNode *node)
{
    Stack *newNode = (Stack*)malloc(sizeof(Stack));
    newNode->node = node;
    newNode->next = *head;
    *head = newNode;
}

TrieNode *pop(Stack **head)
{
    if (*head == NULL)
        return NULL;
    Stack *temp = *head;
    *head = (*head)->next;
    TrieNode *node = temp->node;
    free(temp);
    return node;
}

int isEmpty(Stack *head)
{
    if (head == NULL)
        return 1;
    return 0;
}
