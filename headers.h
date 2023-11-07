#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

#include "trie.h"


#define nms_ss_port 10045
#define nms_client_port 10059
#define ip_address "127.0.0.1"