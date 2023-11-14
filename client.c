#include "headers.h"
#include "requestProtocols.h"
void client()
{
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    buffer[0] = '\0';
    int n;
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
    if (send(mySocket, initialRequest, sizeof(initialRequest), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }

    // Receive acknowledgment
    bzero(buffer, 1024);
    if (recv(mySocket, buffer, sizeof(buffer), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    printf("Acknowledgment received from server: %s\n", buffer);
    while (1)
    {
        // We prompt user for operation number (1 through 6)
        int operationNumber;
        printf("Enter operation number (1-6):\n");
        printf("1. CREATE - Create a new file/folder\n");
        printf("2. READ - Read the content of a file\n");
        printf("3. WRITE - Write data to a file\n");
        printf("4. DELETE - Delete a file/folder\n");
        printf("5. OPEN - Open a file for reading/writing\n");
        printf("6. METADATA - Get metadata information about a file\n");
        if (scanf("%d", &operationNumber) != 1 || operationNumber < 1 || operationNumber > 6)
        {
            fprintf(stderr, "[-]Invalid operation number\n");
            if (close(mySocket) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        // Send operation number to the server
        if (send(mySocket, &operationNumber, sizeof(operationNumber), 0) < 0)
        {
            fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
            if (close(mySocket) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
    }
    closeSocket(mySocket);
    return 0;
}
