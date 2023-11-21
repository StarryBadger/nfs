typedef struct CacheNode
{
    char path[PATH_LIMIT];
    char ip[16];
    int portForClient;
    int portForNM;
    struct CacheNode *prev;
    struct CacheNode *next;
} CacheNode;

typedef struct LRUCache
{
    int maxSize;
    int currentSize;
    CacheNode *head;
    CacheNode *tail;
} LRUCache;
#define LRUSize 16
LRUCache *initLRUCache();
CacheNode *searchCache(LRUCache *cache, const char *path);
void addToCache(LRUCache *cache, const char *path, const char *ip, int portForClient, int portForNM);
void freeCache(LRUCache *cache);
