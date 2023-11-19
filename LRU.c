#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"
cacheSt *createcacheStNode(const char *path, int port, const char *ip)
{
    cacheSt *newNode = (struct cacheSt *)malloc(sizeof(struct cacheSt));
    strcpy(newNode->path, path);
    newNode->port = port;
    strcpy(newNode->ip, ip);
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

cache *createcache()
{
    cache *newcache = (struct cache *)malloc(sizeof(struct cache));
    newcache->head = NULL;
    newcache->tail = NULL;
    newcache->size = 0;
    return newcache;
}

void insertAtEnd(struct cache *cache, struct cacheSt *newNode)
{
    if (cache->size >= MAX_cache_SIZE)
    {
        struct cacheSt *temp = cache->head;
        cache->head = temp->next;
        if (cache->head != NULL)
        {
            cache->head->prev = NULL;
        }
        free(temp);
        cache->size--;
    }
    if (cache->tail == NULL)
    {
        cache->head = newNode;
        cache->tail = newNode;
    }
    else
    {
        newNode->prev = cache->tail;
        cache->tail->next = newNode;
        cache->tail = newNode;
    }
    cache->size++;
}
void moveToEnd(struct cache *cache, struct cacheSt *node)
{
    if (node == cache->tail)
    {
        return;
    }

    if (node == cache->head)
    {
        cache->head = node->next;
        cache->head->prev = NULL;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    node->prev = cache->tail;
    node->next = NULL;
    cache->tail->next = node;
    cache->tail = node;
}
void printcache(struct cache *cache)
{
    struct cacheSt *current = cache->head;
    while (current != NULL)
    {
        printf("Path: %s, Port: %d, IP: %s\n", current->path, current->port, current->ip);
        current = current->next;
    }
}
void freecache(struct cache *cache)
{
    struct cacheSt *current = cache->head;
    struct cacheSt *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    free(cache);
}
