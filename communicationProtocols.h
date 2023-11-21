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

#define nms_ss_port 10045
#define nms_client_port 10065
#define ip_address "127.0.0.1"

#define END_STRING "@@e@*"

#define METADATA_STR_MAX 100
#define SEND_SIZE 1024
#define PATH_LIMIT 1024
#define DIR_LIMIT 500
#define FILE_NAME_LENGTH 100

#define CYN "\e[0;36m"
#define RESET "\x1b[0m"
#define GREEN "\x1b[32m"
#define BLUE "\x1b[34m"
#define WHITE "\x1b[37m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define ORANGE "\e[38;2;255;85;0m"
#define BMAG "\e[1;35m"

typedef struct MessageSS2NM
{
    char buffer[PATH_LIMIT];
    int port_for_clients;
    int port_for_naming_server;
    int port_for_nm_np;
    int port_for_nm_red;
} MessageSS2NM;

typedef struct MessageFormat
{
    char msg[PATH_LIMIT];
    int operation;
    char buffer[PATH_LIMIT];
    bool isADirectory;
    int bytesToRead;
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