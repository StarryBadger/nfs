#include "headers.h"
LRUCache *initLRUCache()
{
    LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
    cache->maxSize = LRUSize;
    cache->currentSize = 0;
    cache->head = NULL;
    cache->tail = NULL;
    return cache;
}
CacheNode *searchCache(LRUCache *cache, const char *path)
{
    CacheNode *current = cache->head;
    while (current != NULL)
    {
        if (strcmp(current->path, path) == 0)
        {
            if (current == cache->head)
            {
                return current;
            }

            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }

            if (current == cache->tail)
            {
                cache->tail = current->prev;
            }

            current->next = cache->head;
            current->prev = NULL;
            cache->head->prev = current;
            cache->head = current;
            return current;
        }
        current = current->next;
    }
    return NULL;
}
void addToCache(LRUCache *cache, const char *path, const char *ip, int portForClient, int portForNM)
{
    if (cache->currentSize >= cache->maxSize)
    {
        CacheNode *tail = cache->tail;
        if (tail != NULL)
        {
            if (tail->prev != NULL)
            {
                tail->prev->next = NULL;
            }
            cache->tail = tail->prev;

            free(tail);
            cache->currentSize--;
        }
    }

    CacheNode *newNode = (CacheNode *)malloc(sizeof(CacheNode));
    strncpy(newNode->path, path, PATH_LIMIT);
    strncpy(newNode->ip, ip, 16);
    newNode->portForClient = portForClient;
    newNode->portForNM = portForNM;
    newNode->next = cache->head;
    newNode->prev = NULL;

    if (cache->head != NULL)
    {
        cache->head->prev = newNode;
    }

    cache->head = newNode;

    if (cache->tail == NULL)
    {
        cache->tail = newNode;
    }

    cache->currentSize++;
}
void freeCache(LRUCache *cache)
{
    CacheNode *current = cache->head;
    while (current != NULL)
    {
        CacheNode *next = current->next;
        free(current);
        current = next;
    }
    free(cache);
}
void deleteNodesWithPort(LRUCache *cache, int port)
{
    CacheNode *current = cache->head;
    CacheNode *next;

    while (current != NULL)
    {
        next = current->next;

        if (current->portForClient == port)
        {
            if (current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                cache->head = current->next;
            }

            if (current->next != NULL)
            {
                current->next->prev = current->prev;
            }
            else
            {
                cache->tail = current->prev;
            }

            free(current);
            cache->currentSize--;
        }

        current = next;
    }
}
