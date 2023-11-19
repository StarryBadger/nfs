#include "headers.h"
errcode closeSocket(int socketToClose)
{
    if (close(socketToClose) < 0)
    {
        fprintf(stderr, RED"[-]Error closing socket: %s\n"RESET, strerror(errno));
        return SOCKET_CLOSE_ERROR;
    }
    return NO_ERROR;
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