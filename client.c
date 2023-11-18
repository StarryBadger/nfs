#include "headers.h"
#include "requestProtocols.h"
void printOperationMessage(MessageClient2NM message)
{
    switch (message.operation)
    {
    case CREATE:
        if (message.isADirectory)
        {
            printf("Selected Operation: CREATE - Create a new directory\n");
        }
        else
        {
            printf("Selected Operation: CREATE - Create a new file\n");
        }
        break;
    case READ:
        printf("Selected Operation: READ - Read the content of a file\n");
        break;
    case WRITE:
        printf("Selected Operation: WRITE - Write data to a file\n");
        break;
    case DELETE:
        if (message.isADirectory)
        {
            printf("Selected Operation: DELETE - Delete an empty directory\n");
        }
        else
        {
            printf("Selected Operation: DELETE - Delete a file\n");
        }
        break;
    case COPY:
        printf("Selected Operation: COPY - Copy file from one directory to another\n");
        break;
    case METADATA:
        printf("Selected Operation: METADATA - Get metadata information about a file\n");
        break;
    default:
        printf("Invalid Operation Number\n");
    }
}
int printConnectionRequest(int acknowledgement)
{
    switch (acknowledgement)
    {
    case INITIAL_ACK_ACCEPT:
        printf("Server has accepted the connection request. Connection established!\n");
        return 1;
    case INITIAL_ACK_UNSUPPORTED_CLIENT:
        printf("Server has rejected the connection request. The client is not supported.\n");
        return 0;
    case INITIAL_ACK_NO_SS_CONNECTED:
        printf("Server has acknowledged the connection, but no Storage Server is currently connected.\n");
        return 0;
    default:
        printf("Unable to infer the server's response. Terminating connection...\n");
    }
    return 0;
}
void printMetadata(metadata *metadata)
{
    printf("I-node number:                      %lu\n", metadata->inodeNumber);
    printf("Mode (permissions: octal):          %lo (octal)\n", metadata->mode);
    printf("Link count:                         %lu\n", metadata->linkCount);
    printf("Ownership:                          UID=%lu   GID=%lu\n", metadata->uid, metadata->gid);
    printf("File size:                          %ld bytes\n", metadata->fileSize);
    printf("Preferred I/O block size:           %ld bytes\n", metadata->preferredBlockSize);
    printf("Blocks allocated:                   %ld\n", metadata->blocksAllocated);
    printf("Last file access:                   %s", ctime(&metadata->lastFileAccess));
    printf("Last file modification:             %s", ctime(&metadata->lastFileModification));
    printf("Last status change:                 %s", ctime(&metadata->lastStatusChange));
    return;
}
errcode handleReadCommunication(int socketSS)
{
    return NO_ERROR;
}
errcode handleWriteCommunication(int socketSS)
{
    char toWrite[PATH_MAX];
    printf("Enter text to be written: ");
    scanf("%s",toWrite);
    if (send(socketSS, &toWrite, sizeof(toWrite), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno)); // ERROR HANDLING
        return NETWORK_ERROR;
    }
    return NO_ERROR;
}
errcode handleMetadataCommunication(int socketSS)
{
    metadata fileInfo;
    if (recv(socketSS, &fileInfo, sizeof(fileInfo), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno)); // ERROR HANDLING
        return NETWORK_ERROR;
    }
    printMetadata(&fileInfo);
    return NO_ERROR;
}
errcode handleSSCommunication(int socketNM, MessageClient2SS message)
{
    int ss_client_port;
    if (recv(socketNM, &ss_client_port, sizeof(ss_client_port), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        return NETWORK_ERROR;
    }
    if (ss_client_port == NO_SUCH_PATH)
    {
        return FILE_NOT_FOUND;
    }
    struct sockaddr_in addr;
    int socketSS = initSocket();
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = ss_client_port;
    addr.sin_addr.s_addr = inet_addr(ip_address);
    if (connect(socketSS, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "[-]Connection error: %s\n", strerror(errno));
        return NETWORK_ERROR;
    }
    printf("Connected to the storage server.\n");
    if (send(socketSS, &message, sizeof(message), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        return NETWORK_ERROR;
    }
    int err_code;
    if (recv(socketSS, &err_code, sizeof(err_code), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        return NETWORK_ERROR;
    }
    if (err_code != NO_ERROR)
    {
        printf("Error code: %d\n", err_code);
        return err_code;
    }
    else
    {
        printf("Operation successful\n");
    }
    switch (message.operation)
    {
    case READ:
        return handleReadCommunication(socketSS);
    case WRITE:
        return handleWriteCommunication(socketSS);
    case METADATA:
        return handleMetadataCommunication(socketSS);
    default:
        return UNEXPECTED_ERROR;
    }
    return NO_ERROR;
}
void askFileOrDirectory(MessageClient2NM *message)
{
    char CHAR_BUFF;
    getchar();
    while (1)
    {
        printf("Enter F for file and D for directory\n");
        scanf("%c", &CHAR_BUFF);
        // getchar();
        if (CHAR_BUFF == 'F' || CHAR_BUFF == 'D' || CHAR_BUFF == 'f' || CHAR_BUFF == 'd')
        {
            message->isADirectory = (CHAR_BUFF == 'D' || CHAR_BUFF == 'd') ? true : false;
            return;
        }
        else
        {
            printf("Invalid request!\n");
        }
    }
}
int main()
{
    struct sockaddr_in addr;
    int mySocket = initSocket();
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = nms_client_port;
    addr.sin_addr.s_addr = inet_addr(ip_address);
    if (connect(mySocket, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "[-]Connection error: %s\n", strerror(errno));
        exit(1);
    }
    printf("Connected to the server.\n");
    // initial request to server
    int initialRequest = INITIAL_MESSAGE;
    if (send(mySocket, &initialRequest, sizeof(initialRequest), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    // acknowledgment
    int initialAck;
    if (recv(mySocket, &initialAck, sizeof(initialAck), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    if (!printConnectionRequest(initialAck))
    {
        closeSocket(mySocket);
        return 0;
    }
    while (1)
    {
        // We prompt user for operation number (1 through 6)
        MessageClient2NM message;
        printf("Enter operation number (1-7):\n");
        printf("1. CREATE - Create a new file/folder\n");
        printf("2. READ - Read the content of a file\n");
        printf("3. WRITE - Write data to a file\n");
        printf("4. DELETE - Delete a file/folder\n");
        printf("5. OPEN - Open a file for reading/writing\n");
        printf("6. METADATA - Get metadata information about a file\n");
        printf("7. TERMINATE - Terminate connection\n");
        if (scanf("%d", &message.operation) != 1 || message.operation < 1 || message.operation > 7)
        {
            fprintf(stderr, "[-]Invalid operation number\n");
            continue;
        }
        if (message.operation == CREATE || message.operation == DELETE)
        {
            askFileOrDirectory(&message);
        }
        printOperationMessage(message);
        if (message.operation != 7)
        {
            printf("Enter file path\n");
            scanf("%s", message.buffer);
        }
        // Send operation number to the server
        if (send(mySocket, &message, sizeof(message), 0) < 0)
        {
            fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
            // close(mySocket);
            // exit(1);
        }
        if (message.operation == TERMINATE)
        {
            break;
        }

        if (message.operation == READ || message.operation == WRITE || message.operation == METADATA)
        {
            if (handleSSCommunication(mySocket, message) != NO_ERROR)
            {
                // do something
            }
        }
    }
    closeSocket(mySocket);
    return 0;
}
