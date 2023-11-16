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
    // Sending initial request to server
    int initialRequest = INITIAL_MESSAGE;
    if (send(mySocket, &initialRequest, sizeof(initialRequest), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }

    // Receive acknowledgment
    int initialAck;
    if (recv(mySocket, &initialAck, sizeof(initialAck), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    printf("Acknowledgment received from server: %d\n", initialAck);

    while (1)
    {
        // We prompt user for operation number (1 through 6)
        int operationNumber;
        printf("Enter operation number (1-7):\n");
        printf("1. CREATE - Create a new file/folder\n");
        printf("2. READ - Read the content of a file\n");
        printf("3. WRITE - Write data to a file\n");
        printf("4. DELETE - Delete a file/folder\n");
        printf("5. OPEN - Open a file for reading/writing\n");
        printf("6. METADATA - Get metadata information about a file\n");
        printf("7. TERMINATE - Terminate connection\n");
        if (scanf("%d", &operationNumber) != 1 || operationNumber < 1 || operationNumber > 7)
        {
            fprintf(stderr, "[-]Invalid operation number\n");
            if (close(mySocket) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        printOperationMessage(operationNumber);
        // Send operation number to the server
        if (send(mySocket, &operationNumber, sizeof(operationNumber), 0) < 0)
        {
            fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
            if (close(mySocket) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        if (operationNumber == 7)
        {
            break;
        }
    }
    closeSocket(mySocket);
}
