#ifndef __REQUEST_PROTOCOLS_H
#define __REQUEST_PROTOCOLS_H
#include <stdint.h>
#include <stdbool.h>
// operations
#define NOT_RECEIVED -1
#define CREATE 1
#define READ 2
#define WRITE 3
#define DELETE 4
#define COPY 5
#define METADATA 6
#define TERMINATE 7

// initial comm
#define INITIAL_MESSAGE 10
#define INITIAL_ACK_ACCEPT 12
#define INITIAL_ACK_UNSUPPORTED_CLIENT 13
#define INITIAL_ACK_NO_SS_CONNECTED 14

#define NO_SUCH_PATH -20

#define nms_ss_port 10040
#define nms_client_port 10051
#define ip_address "127.0.0.1"

#define END_STRING "@@e@*"

#define METADATA_STR_MAX 100
#define SEND_SIZE 4096

typedef struct MessageSS2NM
{
    char buffer[PATH_MAX];
    int port_for_clients;
    int port_for_naming_server;
    int port_for_nm_np;
} MessageSS2NM;

typedef struct MessageFormat
{
    char msg[PATH_MAX];
    int operation;
    char buffer[PATH_MAX];
    bool isADirectory;
} MessageFormat;
typedef MessageFormat MessageNMS2SS_COPY;
typedef MessageFormat MessageNMS2SS_WRITE;

typedef MessageFormat MessageClient2SS;
typedef MessageFormat MessageClient2NM;

typedef struct metadata
{
    uintmax_t inodeNumber;
    uintmax_t mode;
    uintmax_t linkCount;
    uintmax_t uid;
    uintmax_t gid;
    intmax_t preferredBlockSize;
    intmax_t fileSize;
    intmax_t blocksAllocated;
    time_t lastStatusChange;
    time_t lastFileAccess;
    time_t lastFileModification;

} metadata;
#endif