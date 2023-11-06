#include "headers.h"

char buffer[1024];
struct ss_list
{
    int index;
    int ss_port;
    struct ss_list *next;
    struct ss_list *prev;
};
struct ss_list *init_server_list_head()
{
    struct ss_list *head = (struct ss_list *)malloc(sizeof(struct ss_list));
    head->next = NULL;
    head->prev = NULL;
    head->ss_port = -1;
    return head;
}
struct storage_servers_node{
    struct ss_list* head;
    int total_servers;   
};

struct storage_servers_node* storage_servers;

void init_storage_servers()
{
    storage_servers->head=init_server_list_head();
    storage_servers->total_servers=0;
}

void InsertNewSS(int portNo)
{
    struct ss_list* temp;
    temp=storage_servers->head;
    while(temp->next!=NULL)
    temp=temp->next;
    struct ss_list* new=(struct ss_list*)malloc(sizeof(struct ss_list));
    new->next=NULL;
    new->prev=temp;
    temp->next=new;
    new->index=storage_servers->total_servers+1;
    storage_servers->total_servers++;
    new->ss_port=portNo;
    return;
}
void RemoveSS(int portNo)
{
    if(storage_servers->total_servers==0)
    return;
    struct ss_list* temp;
    temp=storage_servers->head->next;
    while(temp!=NULL)
    {
        if(temp->ss_port==portNo)
        {
            temp->prev->next=temp->next;
            temp->next->prev=temp->prev;
            storage_servers->total_servers--;
            break;
        }
        temp=temp->next;
    }
    if(temp!=NULL)
    free(temp);
    return;
}
int close_signal=0;
void *ss_port_worker(void *arg)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    //   char buffer[1024];
    int n;
    // char port_str[6];
    // strcpy(port_str, argv[1]);
    // int port = atoi(port_str);
    // printf("%d\n", port);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
        exit(1);
    }
    // printf("[+]TCP server socket created.\n");

    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = nms_ss_port;
    server_addr.sin_addr.s_addr = inet_addr(ip_address);

    n = bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (n < 0)
    {
        fprintf(stderr, "[-]Bind error: %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    // printf("[+]Bind to the port number: %d\n", nms_ss_port);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Listen error: %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    // printf("Listening...\n");
     bzero(buffer, 1024);
    if(recv(client_sock, buffer, sizeof(buffer), 0)<0)
    {
      fprintf(stderr,"[-]Receive error: %s\n",strerror(errno));
      if(close(client_sock)<0)
      fprintf(stderr,"[-]Error closing socket: %s\n",strerror(errno));
      if(close(server_sock)<0)
      fprintf(stderr,"[-]Error closing socket: %s\n",strerror(errno));
      exit(1);
    }
}

void ss_is_alive_checker()
{
    // struct ss_list*ss_list_head;
    struct ss_list* temp;
    // ss_list_head = (struct ss_list *)arg;
    temp = storage_servers->head->next;
    while (temp != NULL)
    {
        int shrey_sock;
        struct sockaddr_in addr;
        socklen_t addr_size;
        // char buffer[1024];
        int n;

        shrey_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (shrey_sock < 0)
        {
            fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));

            // exit(1);
        }
        // printf("[+]TCP server socket created.\n");

        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = nms_ss_port;
        addr.sin_addr.s_addr = inet_addr(ip_address);

        if (connect(shrey_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            fprintf(stderr, "[-]Connection lost from storage server with port %d: %s\n",temp->ss_port, strerror(errno));
            // exit(1);
        }
        temp=temp->next;
    }
}
void* ss_is_alive_worker(void* arg)
{
    while(1)
    {
        ss_is_alive_checker();
        if(close_signal==1)
        return;
    }
}
int main(int argc, char *argv[])
{

    pthread_t ss_port, ss_is_alive, client_connection;
}