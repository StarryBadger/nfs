typedef struct CacheNode
{
    char path[PATH_MAX];
    char ip[16];
    int port;
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
void addToCache(LRUCache *cache, const char *path, const char *ip, int port);
void freeCache(LRUCache *cache);
