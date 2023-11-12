#include "headers.h"
int main()
{
    int mySocket;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    buffer[0] = '\0';
    int n;
    mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
        exit(1);
    }
    printf("[+]TCP socket created.\n");
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
    printf("Enter message to send: ");
    if (scanf("%1023[^\n]", buffer) != 1)
    {
        fprintf(stderr, "[-]Error reading input");
        exit(1);
    }
    printf("Message being sent to server: %s\n", buffer);
    if (send(mySocket, buffer, strlen(buffer), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    bzero(buffer, 1024);
    if (recv(mySocket, buffer, sizeof(buffer), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
        if (close(mySocket) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    printf("Message received from server: %s\n", buffer);

    if (close(mySocket) < 0)
    {
        fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    return 0;
}