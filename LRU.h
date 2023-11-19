#define MAX_cache_SIZE 2
typedef struct cacheSt
{
    char path[256];
    int port;
    char ip[16];
    struct cacheSt *next;
    struct cacheSt *prev;
}cacheSt;

typedef struct cache
{
    struct cacheSt *head;
    struct cacheSt *tail;
    int size;
}cache;
cacheSt *createcacheStNode(const char *path, int port, const char *ip);
cache *createcache();
void insertAtEnd(struct cache *cache, struct cacheSt *newNode);
void moveToEnd(struct cache *cache, struct cacheSt *node);
void printcache(struct cache *cache);
void freecache(struct cache *cache);
