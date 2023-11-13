#include "headers.h"
void closeSocket(int socketToClose)
{
    if (close(socketToClose) < 0)
    {
        fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
    }
    exit(SOCKET_CLOSE_ERROR);
}
int initSocket()
{
    int socketCreated = socket(AF_INET, SOCK_STREAM, 0);
    if (socketCreated < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
        exit(SOCKET_INIT_ERROR);
    }
    printf("[+]TCP socket created.\n");
    return socketCreated;
}