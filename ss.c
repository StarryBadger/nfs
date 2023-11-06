#include "headers.h"

int port_for_clients;
int port_for_naming_server;
int close_flag=0;
void *naming_server_responder_worker(void *arg)
{
    while (1)
    {
        int server_sock, client_sock;
        struct sockaddr_in server_addr, client_addr;
        socklen_t addr_size;
        char buffer[1024];
        int n;

        server_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (server_sock < 0)
        {
            fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
            // exit(1);
        }
        // printf("[+]TCP server socket created.\n");

        memset(&server_addr, '\0', sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = port_for_naming_server;
        server_addr.sin_addr.s_addr = inet_addr(ip_address);

        n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (n < 0)
        {
            fprintf(stderr, "[-]Bind error: %s\n", strerror(errno));
            close(server_sock);
            // exit(1);
        }
        // printf("[+]Bind to the port number: %d\n", port_for_naming_server);
        if (listen(server_sock, 5) < 0)
        {
            fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
            close(server_sock);
            exit(1);
        }
        // printf("Listening...\n");
        if(close_flag==1)
        return;
    }
}
void *naming_server_informer_worker(void *arg)
{
    int ss_sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    ss_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ss_sock < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));

        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = nms_ss_port;
    addr.sin_addr.s_addr = inet_addr(ip_address);

    if (connect(ss_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "[-]Connection time error: %s\n", strerror(errno));
        exit(1);
    }
    printf("Connected to the server.\n");

    bzero(buffer, 1024);
    printf("Your message: ");
    scanf("%[^\n]", buffer);
    printf("Sending message to server: %s\n", buffer);

    if (send(ss_sock, buffer, strlen(buffer), 0) < 0)
    {
        fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
        if (close(ss_sock) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
}

int main()
{
    pthread_t clients_handler, naming_server_informer, naming_server_responder;
}