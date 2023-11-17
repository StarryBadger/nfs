#include "headers.h"
#include "requestProtocols.h"
void printOperationMessage(int operationNumber)
{
    switch (operationNumber)
    {
    case CREATE:
        printf("Selected Operation: CREATE - Create a new file/folder\n");
        break;
    case READ:
        printf("Selected Operation: READ - Read the content of a file\n");
        break;
    case WRITE:
        printf("Selected Operation: WRITE - Write data to a file\n");
        break;
    case DELETE:
        printf("Selected Operation: DELETE - Delete a file/folder\n");
        break;
    case OPEN:
        printf("Selected Operation: OPEN - Open a file for reading/writing\n");
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
        printOperationMessage(message.operation);
        if (message.operation != 7)
        {
            printf("Enter file path\n");
            scanf("%s", message.buffer);
        }
        // Send operation number to the server
        if (send(mySocket, &message, sizeof(message), 0) < 0)
        {
            fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
            close(mySocket);
            exit(1);
        }
        if (message.operation == 7)
        {
            break;
        }
    }
    closeSocket(mySocket);
    return 0;
}
