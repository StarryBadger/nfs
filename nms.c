#include "headers.h"
char logfile[30];
int can_enter = 0;
struct ss_list
{
    int index;
    int ssToc_port;
    int ssTonms_port;
    int ssTonmnp_port;
    struct ss_list *next;
    struct ss_list *prev;
    TrieNode *root;
};

struct ss_list *init_server_list_head()
{
    struct ss_list *head = (struct ss_list *)malloc(sizeof(struct ss_list));
    head->next = NULL;
    head->prev = NULL;
    head->ssToc_port = -1;
    head->ssTonms_port = -1;
    head->index = -1;
    head->root = NULL;
    return head;
}

struct storage_servers_node
{
    struct ss_list *head;
    int total_servers;
};

struct storage_servers_node *storage_servers;

int search_port(char *buffer)
{
    struct ss_list *temp;
    temp = storage_servers->head->next;
    TrieNode *searchResult;
    while (temp != NULL)
    {
        searchResult = SearchTrie(buffer, temp->root);
        if (searchResult != NULL)
        {
            if (searchResult->isAccessible)
            {
                return temp->ssToc_port;
            }
        }
        temp = temp->next;
    }
    return NO_SUCH_PATH;
}
int search_port2(char *buffer)
{
    struct ss_list *temp;
    temp = storage_servers->head->next;
    TrieNode *searchResult;
    while (temp != NULL)
    {
        searchResult = SearchTrie(buffer, temp->root);
        if (searchResult != NULL)
        {
            if (searchResult->isAccessible)
            {
                return temp->ssTonmnp_port;
            }
        }
        temp = temp->next;
    }
    return NO_SUCH_PATH;
}
void init_storage_servers()
{
    storage_servers->head = init_server_list_head();
    storage_servers->total_servers = 0;
    // sem_post(&initial_lock);
    can_enter = 1;
}

void InsertNewSS(int ssTocPortNo, int ssTonmsPortNo, int ssToNMmpport, TrieNode *ssRoot)
{
    struct ss_list *temp;
    temp = storage_servers->head;
    while (temp->next != NULL)
        temp = temp->next;
    struct ss_list *new = (struct ss_list *)malloc(sizeof(struct ss_list));
    new->next = NULL;
    new->prev = temp;
    temp->next = new;
    new->index = storage_servers->total_servers + 1;
    storage_servers->total_servers++;
    new->ssToc_port = ssTocPortNo;
    new->ssTonms_port = ssTonmsPortNo;
    new->ssTonmnp_port = ssToNMmpport;
    new->root = ssRoot;
    PrintTrieLIkeAnActualTRee(new->root, 4);
    return;
}
void RemoveSS(int index)
{
    if (storage_servers->total_servers == 0)
        return;
    struct ss_list *temp;
    temp = storage_servers->head->next;
    while (temp != NULL)
    {
        if (temp->index == index)
        {
            if (temp->prev != NULL)
                temp->prev->next = temp->next;
            if (temp->next != NULL)
                temp->next->prev = temp->prev;
            storage_servers->total_servers--;
            struct ss_list *temp2;
            temp2 = temp;
            temp = temp->next;
            free(temp2);
            continue;
        }
        else if (temp->index > index)
        {
            temp->index--;
        }
        temp = temp->next;
    }
    return;
}

int close_signal = 0;

void *ss_port_worker(void *arg)
{
    // sem_wait(&initial_lock);
    while (1)
    {
        if (can_enter == 1)
            break;
    }
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    //   char buffer[PATH_MAX];
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
    printf("[+]Bind to the port number: %d\n", nms_ss_port);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Listen error: %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    printf("Listening...\n");
    char buffer[PATH_MAX];
    while (1)
    {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0)
        {
            fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
            if (close(server_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        printf("[+]Storage server connected.\n");

        bzero(buffer, PATH_MAX);
        MessageSS2NM message;
        if (recv(client_sock, &message, sizeof(message), 0) < 0)
        {
            fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            if (close(server_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        printf("Message from storage server: %s\n", message.buffer);
        printf("Port for clients: %d\n", message.port_for_clients);
        printf("Port for nms: %d\n", message.port_for_naming_server);
        printf("Port for nm_np: %d\n", message.port_for_nm_np);

        // PrintTrie(StringToTrie(message.buffer));
        InsertNewSS(message.port_for_clients, message.port_for_naming_server, message.port_for_nm_np, StringToTrie(message.buffer));
    }
}

void ss_is_alive_checker()
{
    // struct ss_list*ss_list_head;
    struct ss_list *temp;
    // ss_list_head = (struct ss_list *)arg;
    temp = storage_servers->head->next;
    while (temp != NULL)
    {
        int sock;
        struct sockaddr_in addr;
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));

            // exit(1);
        }
        // printf("[+]TCP server socket created.\n");

        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        // printf("%d\n",temp->ssTonms_port);
        addr.sin_port = temp->ssTonms_port;
        addr.sin_addr.s_addr = inet_addr(ip_address);

        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            fprintf(stderr, "[-]Connection lost from storage server with port %d: %s\n", temp->ssTonms_port, strerror(errno));
            RemoveSS(temp->index);
            // exit(1);
        }
        // else{
        //  printf("pinged storage server with port %d once\n",temp->ssTonms_port);
        // }
        close(sock);
        temp = temp->next;
    }
}
char *pathString(char **path_line, int size, int start)
{
    char *p_string = (char *)malloc(sizeof(char) * 4096);
    p_string[0] = '\0';
    int total_len = 0;
    for (int i = start; i < size; i++)
    {
        int len = strlen(path_line[i]);
        total_len += len;
        strcat(p_string, path_line[i]);
        if (i != size - 1)
        {
            strcat(p_string, "/");
            total_len++;
        }
        p_string[total_len] = '\0';
    }
    return p_string;
}
void lessgoRec_again(int sock, char **path_line, int index, TrieNode *node, char *path)
{
    if (node == NULL)
        return;
    strcpy(path_line[index], node->directory);
    if (node->isFile == 0)
    {
        if (node->firstChild)
        {
            lessgoRec_again(sock, path_line, index + 1, node->firstChild, path);
        }
    }
    if (node->sibling)
    {
        path_line[index][0] = '\0';
        lessgoRec_again(sock, path_line, index, node->sibling, path);
    }

    MessageFormat msg;
    msg.operation = DELETE;
    char temp_dest_path[PATH_MAX];
    strcpy(temp_dest_path, path);
    strcat(temp_dest_path, "/");
    strcat(temp_dest_path, pathString(path_line, index + 1, initial_index));
    if (send(sock, &msg, sizeof(msg), 0) < 0)
    {
        fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
        // if (close(sock) < 0)
        //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        return;
    }
    path_line[index][0]='\0';
    return;
}
void lessgoRec(int sock, int sock2, char **path_line, int index, TrieNode *node, int initial_index, char *dest_path, int level_flag)
{
    if (node == NULL)
        return;
    strcpy(path_line[index], node->directory);
    MessageClient2NM msg;
    if (node->isFile == 1)
    {
        strcpy(msg.buffer, pathString(path_line, index + 1, 0));
        msg.operation = READ;
        printf("Sending message to server to read: %s %d\n", msg.buffer, msg.operation);

        if (send(sock, &msg, sizeof(msg), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
            // if (close(sock) < 0)
            //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            return;
        }
        char buffer[PATH_MAX];
        bzero(buffer, PATH_MAX);
        char temp_dest_path[PATH_MAX];
        strcpy(temp_dest_path, dest_path);
        strcat(temp_dest_path, "/");
        strcat(temp_dest_path, pathString(path_line, index + 1, initial_index));

        strcpy(msg.buffer, temp_dest_path);
        msg.isADirectory = 0;
        msg.operation = CREATE;
        printf("Sending message to server to read: %s %d\n", msg.buffer, msg.operation);

        if (send(sock2, &msg, sizeof(msg), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
            // if (close(sock) < 0)
            //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            return;
        }
        int bytesread;
        int send_cout = 0;
        // FILE* this = fopen("this_nm.txt","w");
        MessageFormat message_read;
        bzero(message_read.msg, PATH_MAX);
        while ((bytesread = recv(sock, &message_read, sizeof(message_read), 0)) > 0)
        {
            // fwrite(buffer,1,bytesread,this);
            MessageNMS2SS_COPY msg_to_send;
            // printf("Received message from server: %s\n", buffer);
            strcpy(msg_to_send.msg, message_read.msg);
            // msg_to_send.msg[bytesread] = '\0';
            // for(int l = bytesread;l<PATH_MAX;l++)
            // {
            //     msg_to_send.msg[l] = '\0';
            // }
            // strncpy(msg_to_send.msg, buffer, bytesread);

            msg_to_send.operation = WRITE;

            // printf("Sending message to server to write: %s  %s  %d\n", msg_to_send.buffer, msg_to_send.msg, msg_to_send.operation);

            strcpy(msg_to_send.buffer, temp_dest_path);

            msg_to_send.bytesToRead = message_read.bytesToRead;
            // send_cout++;
            // printf("send count: %d\n",send_cout);
            // if(send_cout>24)
            // {
            //     break;
            // }            // printf("Sending message to server to write: %s  %s  %d\n", msg_to_send.buffer, msg_to_send.msg, msg_to_send.operation);

            if (send(sock2, &msg_to_send, sizeof(msg_to_send), 0) < 0)
            {
                fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
                // if (close(sock) < 0)
                //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
                return;
            }
            bzero(buffer, PATH_MAX);
            if (message_read.bytesToRead < SEND_SIZE)
            {
                break;
            }
        }
        if (bytesread <= 0)
        {
            printf("atleasat its here\n");
        }
    }
    else
    {
        // strcpy(msg.buffer, pathString(path_line, index + 1,initial_index));
        char temp_dest_path[PATH_MAX];
        strcpy(temp_dest_path, dest_path);
        strcat(temp_dest_path, "/");
        strcat(temp_dest_path, pathString(path_line, index + 1, initial_index));

        strcpy(msg.buffer, temp_dest_path);
        msg.isADirectory = 1;
        msg.operation = CREATE;
        printf("Sending message to server to read: %s %d\n", msg.buffer, msg.operation);

        if (send(sock2, &msg, sizeof(msg), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
            // if (close(sock) < 0)
            //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            return;
        }
    }

    if (node->firstChild)
    {
        lessgoRec(sock, sock2, path_line, index + 1, node->firstChild, initial_index, dest_path, 0);
    }
    if (level_flag == 0)
    {
        path_line[index][0] = '\0';
        if (node->sibling)
            lessgoRec(sock, sock2, path_line, index, node->sibling, initial_index, dest_path, level_flag);
    }
    // printf("here with directory : %s\n",node->directory);
    path_line[index][0] = '\0';
    return;
}

void *ss_is_alive_worker(void *arg)
{
    while (1)
    {

        if (can_enter == 1)
        {
            break;
        }
    }
    while (1)
    {
        ss_is_alive_checker();
        if (close_signal == 1)
            return NULL;
    }
    return NULL;
}

void CopyPath2Path(char *src_path, char *dest_path)
{
    int sock, sock2;
    int port1, port2;
    port1 = search_port2(src_path);
    port2 = search_port2(dest_path);
    struct sockaddr_in addr, addr2;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sock2 = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));

        exit(1);
    }
    if (sock2 < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));

        exit(1);
    }

    // printf("[+]TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    // printf("%d\n",temp->ssTonms_port);
    addr.sin_port = port1;
    addr.sin_addr.s_addr = inet_addr(ip_address);

    memset(&addr2, '\0', sizeof(addr2));
    addr2.sin_family = AF_INET;
    // printf("%d\n",temp->ssTonms_port);
    addr2.sin_port = port2;
    addr2.sin_addr.s_addr = inet_addr(ip_address);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "[-]Couldn't connect to storage server 1: %s\n", strerror(errno));
        exit(1);
    }
    if (connect(sock2, (struct sockaddr *)&addr2, sizeof(addr2)) < 0)
    {
        fprintf(stderr, "[-]Couldn't connect to storage server 2: %s\n", strerror(errno));
        exit(1);
    }
    char **path_line = (char **)malloc(sizeof(char *) * 500);
    for (int i = 0; i < 500; i++)
    {
        path_line[i] = (char *)malloc(sizeof(char) * 100);
        path_line[i][0] = '\0';
    }

    TrieNode *node = NULL;
    struct ss_list *temp;
    temp = storage_servers->head->next;
    while (temp != NULL)
    {
        if ((node = SearchTrie(src_path, temp->root)) != NULL)
        {
            break;
        }
        temp = temp->next;
    }
    if (!node)
    {
        printf("invalid source path\n");
        return;
    }
    char temp_buff[PATH_MAX];
    strcpy(temp_buff, src_path);
    char del2[] = "/";
    char *token2;
    char *ptr_in2;
    int path_count = 0;
    token2 = __strtok_r(temp_buff, del2, &ptr_in2);
    while (token2 != NULL)
    {
        strcpy(path_line[path_count], token2);
        path_count++;
        token2 = __strtok_r(NULL, del2, &ptr_in2);
    }
    int initial_index = path_count - 1;
    path_line[path_count - 1][0] = '\0';
    // printf("hi1\n");
    lessgoRec(sock, sock2, path_line, path_count - 1, node, initial_index, dest_path, 1);
}

void *client_handler(void *arg)
{
    int clientSocket = *((int *)arg);
    int initialRequest, initialAck;
    MessageClient2NM message;
    int terminateConnectionFlag = 0;
    if (recv(clientSocket, &initialRequest, sizeof(initialRequest), 0) < 0)
    {
        fprintf(stderr, RED "[-]Receive error: %s\n" RESET, strerror(errno));
        logThis(logfile, LOG_ERROR, CLIENT_NM, "Initial Request: %s", strerror(errno));
        if (closeSocket(clientSocket) == NO_ERROR)
        {
        }
    }
    printf("Initial Request received from client: %d\n", initialRequest);
    logThis(logfile, LOG_INFO, CLIENT_NM, "Initial Request %d [PORT: %d IP:%d]", initialRequest, nms_client_port, ip_address);
    if (initialRequest == INITIAL_MESSAGE)
    {
        initialAck = INITIAL_ACK_ACCEPT;
    }
    else
    {
        initialAck = INITIAL_ACK_UNSUPPORTED_CLIENT;
        terminateConnectionFlag = 1;
    }
    // add no ss case
    if (send(clientSocket, &initialAck, sizeof(initialAck), 0) < 0)
    {
        fprintf(stderr, "[-]Send error: %s\n", strerror(errno));
        close(clientSocket);
        return NULL;
    }
    logThis(logfile, LOG_INFO, NM_CLIENT, "Initial Acknowledgement %d [PORT: %d IP: %d]", initialAck, nms_client_port, ip_address);
    printf("Acknowledgment sent to client: %d\n", initialAck);
    while (1)
    {
        message.operation = NOT_RECEIVED;
        if (recv(clientSocket, &message, sizeof(message), 0) < 0)
        {
            fprintf(stderr, "[-]Receive error: %s\n", strerror(errno));
            close(clientSocket);
            return NULL;
        }
        logOperation(logfile, message);
        printf("received %s %d\n", message.buffer, message.operation);
        if (message.operation >= 7 || message.operation < 1)
        {
            terminateConnectionFlag = 1;
        }
        // add send code here
        if (terminateConnectionFlag)
        {
            break;
        }
        int port_to_send;
        if (message.operation == READ || message.operation == WRITE || message.operation == METADATA)
        {
            port_to_send = search_port(message.buffer);
            if (send(clientSocket, &port_to_send, sizeof(port_to_send), 0) < 0)
            {
                fprintf(stderr, "[-]Sendtime error: %s\n", strerror(errno));
            }
            if (port_to_send == NO_SUCH_PATH)
            {
                printf("Client entered an invalid/inaccessible path\n");
                // logThis(logfile,)
                continue;
            }
            printf("Port sent to client %d\n", port_to_send);
            // log
        }
        else if (message.operation == CREATE || message.operation == DELETE)
        {
            struct ss_list *temp;
            int validpath = 0;
            int port_to_ss;
            temp = storage_servers->head->next;
            while (temp != NULL)
            {
                if (((message.operation == CREATE) && SearchTrie(PathParent(message.buffer), temp->root) != NULL) || (message.operation == DELETE && SearchTrie(message.buffer, temp->root) != NULL))
                {
                    if ((message.operation == CREATE && !SearchTrie(PathParent(message.buffer), temp->root)->isAccessible) || (message.operation == DELETE && !SearchTrie(message.buffer, temp->root)->isAccessible))
                        break;
                    port_to_ss = temp->ssTonmnp_port;
                    validpath = 1;
                    break;
                }
                temp = temp->next;
            }
            if ((message.operation == CREATE) && strcmp(message.buffer, PathParent(message.buffer)) == 0)
            {
                validpath = 1;
                temp = storage_servers->head->next;
                if (temp != NULL)
                    port_to_ss = temp->ssTonmnp_port;
                else
                    validpath = 0;
            }
            if (!validpath)
            {
                printf("no valid path\n");
                int err_code_about_to_send = NO_SUCH_PATH;
                if (send(clientSocket, &err_code_about_to_send, sizeof(err_code_about_to_send), 0) < 0)
                {
                    fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                    // exit(1);
                    continue;
                }
            }
            else
            {
                printf("valid path\n");
                int nms_sock;
                struct sockaddr_in addr;
                socklen_t addr_size;
                int n;
                nms_sock = socket(AF_INET, SOCK_STREAM, 0);
                if (nms_sock < 0)
                {
                    fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
                }
                printf("[+]TCP server socket created.\n");

                memset(&addr, '\0', sizeof(addr));
                addr.sin_family = AF_INET;
                addr.sin_port = port_to_ss;
                addr.sin_addr.s_addr = inet_addr(ip_address);

                if (connect(nms_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
                {
                    fprintf(stderr, "[-]Connection time error: %s\n", strerror(errno));
                }
                printf("Connected to the SS.\n");
                printf("Sending message to server: %d %s\n", message.operation, message.buffer);

                if (send(nms_sock, &message, sizeof(message), 0) < 0)
                {
                    fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
                    if (close(nms_sock) < 0)
                        fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
                }

                int err_code_about_to_send;
                if (recv(nms_sock, &err_code_about_to_send, sizeof(err_code_about_to_send), 0) < 0)
                {
                    fprintf(stderr, "[-]Receive time error: %s\n", strerror(errno));
                    // return;
                }
                printf("Error code received from storage server: %d\n", err_code_about_to_send);
                if (err_code_about_to_send == NO_ERROR)
                {
                    if (message.operation == CREATE)
                    {
                        temp = storage_servers->head->next;
                        while (temp != NULL)
                        {
                            if (SearchTrie(PathParent(message.buffer), temp->root) != NULL)
                            {
                                printf("IN CREATE\n");
                                InsertTrie(message.buffer, temp->root, (int)(!message.isADirectory), 1);
                                PrintTrieLIkeAnActualTRee(temp->root, 4);
                                break;
                            }
                            temp = temp->next;
                        }
                        if (strcmp(message.buffer, PathParent(message.buffer)) == 0)
                        {
                            temp = storage_servers->head->next;
                            printf("IN CREATE\n");
                            InsertTrie(message.buffer, temp->root, (int)(!message.isADirectory), 1);
                            PrintTrieLIkeAnActualTRee(temp->root, 4);
                        }
                    }
                    else if (message.operation == DELETE)
                    {
                        temp = storage_servers->head->next;
                        while (temp != NULL)
                        {
                            if (SearchTrie(message.buffer, temp->root) != NULL)
                            {
                                DeleteTrie(message.buffer, temp->root);
                                PrintTrieLIkeAnActualTRee(temp->root, 4);
                                break;
                            }
                            temp = temp->next;
                        }
                    }
                }
                close(nms_sock);
                if (send(clientSocket, &err_code_about_to_send, sizeof(err_code_about_to_send), 0) < 0)
                {
                    fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                    // exit(1);
                    continue;
                }
            }
        }
        else if (message.operation == COPY)
        {
            CopyPath2Path(message.buffer, message.msg);
        }
    }
    close(clientSocket);
    printf("Client disconnected\n");
    return NULL;
}

void *client_connection_worker(void *arg)
{
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrSize;

    serverSocket = initSocket();
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = nms_client_port;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        fprintf(stderr, "[-]Bind error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, SOMAXCONN) < 0)
    {
        fprintf(stderr, "[-]Listen error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Client Connection Thread: Listening for clients on port %d\n", nms_client_port);
    while (1)
    {
        addrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
        if (clientSocket < 0)
        {
            fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
            continue;
        }
        pthread_t clientThread;
        pthread_create(&clientThread, NULL, client_handler, (void *)&clientSocket);
    }
    close(serverSocket);
    return NULL;
}

int main(int argc, char *argv[])
{
    storage_servers = (struct storage_servers_node *)malloc(sizeof(struct storage_servers_node));
    init_storage_servers();
    if (initLog(logfile) != NO_ERROR)
    {
        fprintf(stderr, RED "Unable to create log file.\n" RESET);
        exit(FILE_DESCRIPTOR_ERROR);
    }
    pthread_t ss_port, ss_is_alive, client_connection;
    pthread_create(&ss_port, NULL, ss_port_worker, NULL);
    pthread_create(&ss_is_alive, NULL, ss_is_alive_worker, NULL);
    pthread_create(&client_connection, NULL, client_connection_worker, NULL);
    while (1)
    {
    }
}