#include "headers.h"

int port_for_clients;
int port_for_naming_server;
sem_t portc_lock;
sem_t portnms_lock;
int close_flag = 0;
void *naming_server_responder_worker(void *arg)
{

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // char buffer[1024];
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
    server_addr.sin_port = 0;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        fprintf(stderr, "[-]Bind error: %s\n", strerror(errno));
        close(server_sock);
        // exit(1);
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    while (1)
    {
        if (getsockname(server_sock, (struct sockaddr *)&sin, &len) == -1)
        {
            fprintf(stderr, "couldn't extract port of socket error\n");
            continue;
        }
        else
            break;
    }
    port_for_naming_server = ntohs(sin.sin_port);
    sem_post(&portnms_lock);
    // printf("[+]Bind to the port number: %d\n", port_for_naming_server);
    while (1)
    {
        if (listen(server_sock, 5) < 0)
        {
            fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
            close(server_sock);
            exit(1);
        }
        // printf("Listening...\n");
        if (close_flag == 1)
            return NULL;
    }
    return NULL;
}
void *naming_server_informer_worker(void *arg)
{
    sem_wait(&portc_lock);
    sem_wait(&portnms_lock);
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
    sprintf(buffer,"%d,%d",port_for_clients,port_for_naming_server);
    printf("Sending message to server: %s\n", buffer);

    if (send(ss_sock, buffer, strlen(buffer), 0) < 0)
    {
        fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
        if (close(ss_sock) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }
    return NULL;

}

void* clients_handler_worker(void* arg)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // char buffer[1024];
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
    server_addr.sin_port = 0;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        fprintf(stderr, "[-]Bind error: %s\n", strerror(errno));
        close(server_sock);
        // exit(1);
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    while (1)
    {
        if (getsockname(server_sock, (struct sockaddr *)&sin, &len) == -1)
        {
            fprintf(stderr, "couldn't extract port of socket error\n");
            continue;
        }
        else
            break;
    }
    port_for_clients = ntohs(sin.sin_port);
    sem_post(&portc_lock);
    //listen for new clients and make a new thread if a client is found and do ss-client communication part in the worker function of that new thread
    while(1)
    {}
    return NULL;

}

int main()
{
    pthread_t clients_handler, naming_server_informer, naming_server_responder;
    sem_init(&portc_lock,0,0);
    sem_init(&portnms_lock,0,0);
    pthread_create(&clients_handler,NULL,clients_handler_worker,NULL);
    pthread_create(&naming_server_responder,NULL,naming_server_responder_worker,NULL);
    pthread_create(&naming_server_informer,NULL,naming_server_informer_worker,NULL);
    while(1)
    {

    }
}