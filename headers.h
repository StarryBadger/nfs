#include <arpa/inet.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <linux/limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "trie.h"

#define MAX_PATH_LENGTH 1024
#define nms_ss_port 10045
#define nms_client_port 10059
#define ip_address "127.0.0.1"

typedef struct MessageSS2NM
{
    char buffer[MAX_PATH_LENGTH];
    int port_for_clients;
    int port_for_naming_server;
} MessageSS2NM;