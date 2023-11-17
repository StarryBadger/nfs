#ifndef __STACK_H
#define __STACK_H

#include "headers.h"

typedef struct Stack
{
    TrieNode *node;
    struct Stack *next;
} Stack;

void push(Stack **head, TrieNode *node);

TrieNode *pop(Stack **head);

int isEmpty(Stack *head);


#endif