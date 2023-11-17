#ifndef __REQUEST_PROTOCOLS_H
#define __REQUEST_PROTOCOLS_H
#include <stdint.h>
// operations
#define CREATE 1
#define READ 2
#define WRITE 3
#define DELETE 4
#define OPEN 5
#define METADATA 6

// initial comm
#define INITIAL_MESSAGE 10
#define INITIAL_ACK_ACCEPT 12
#define INITIAL_ACK_UNSUPPORTED_CLIENT 13
#define INITIAL_ACK_NO_SS_CONNECTED 14

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

typedef struct MessageClient
{
    char buffer[MAX_PATH_LENGTH];
    int operation;
} MessageClient;
typedef MessageClient MessageClient2SS;
typedef MessageClient MessageClient2NM;
typedef struct metadata
{
    uint64_t fileSize;    // Size of the file in bytes
    uint32_t permissions; // File permissions
    uint64_t createTime;  // Timestamp: when the file was created
    uint64_t modifyTime;  // Timestamp: the last modification time
}metadata;
#endif