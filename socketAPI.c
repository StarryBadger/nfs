#include "headers.h"
void closeSocket(int socketToClose)
{
    if (close(socketToClose) < 0)
    {
        fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
    }
    exit(1);
}
int initSocket()
{
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mySocket < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
        exit(1);
    }
}