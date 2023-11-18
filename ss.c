#include "headers.h"

int port_for_clients;
int port_for_naming_server;
int port_for_naming_server_np;
sem_t portc_lock;
sem_t portnms_lock;
sem_t portnmsNp_lock;
int close_flag = 0;
TrieNode *ssTrie = NULL;
pthread_t nm_ss_connection;

void *NMServerConnection(void *arg);

void *naming_server_responder_worker(void *arg)
{
    // printf("here i am\n");
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // char buffer[MAX_PATH_LENGTH];
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
    // port_for_naming_server = ntohs(sin.sin_port);
    // printf("port extracted is %d\n",sin.sin_port);
    port_for_naming_server = sin.sin_port;
    sem_post(&portnms_lock);
    // printf("[+]Bind to the port number: %d\n", port_for_naming_server);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    pthread_create(&nm_ss_connection, NULL, NMServerConnection, (void *)&server_sock);

    printf("listening to respond to naming server\n");
    while (1)
    {

        addr_size = sizeof(client_addr);
        // printf("1\n");
        // printf("%d\n",port_for_naming_server);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        // printf("2\n");
        if (client_sock < 0)
        {
            fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
            if (close(server_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            // exit(1);
        }

        close(client_sock);
        if (close_flag == 1)
            return NULL;
    }
    return NULL;
}

void *naming_server_informer_worker(void *arg)
{
    sem_wait(&portc_lock);
    sem_wait(&portnms_lock);
    sem_wait(&portnmsNp_lock);
    int ss_sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[MAX_PATH_LENGTH];
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

    // ssTrie = createNode("storage_server_1");
    // InsertTrie("root/dir1/dir2/dir3",ssTrie);
    // InsertTrie("root/dir1/dir5/dir6",ssTrie);
    // InsertTrie("root/dir1/dir2/dir4",ssTrie);
    // InsertTrie("root/dir1/dir5/dir7",ssTrie);
    // InsertTrie("root/dir2/dir6/dir9",ssTrie);
    // InsertTrie("root/dir2/dir6/dir10",ssTrie);

    // bzero(buffer, MAX_PATH_LENGTH);
    // sprintf(buffer,"%d,%d",port_for_clients,port_for_naming_server);
    MessageSS2NM message;
    // strcpy(message.buffer, "storage_server_1");
    TrieToString(ssTrie, message.buffer);
    message.port_for_clients = port_for_clients;
    message.port_for_naming_server = port_for_naming_server;
    message.port_for_nm_np = port_for_naming_server_np;

    printf("Sending message to server: %d %d %d\n", message.port_for_clients, message.port_for_naming_server, message.port_for_nm_np);

    if (send(ss_sock, &message, sizeof(message), 0) < 0)
    {
        fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
        if (close(ss_sock) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
        exit(1);
    }

    return NULL;
}

void *CLientServerConnection(void *arg)
{
    int client_sock = *(int *)arg;
    int err_code = NO_ERROR;
    MessageClient2SS message;
    bzero(message.buffer, MAX_PATH_LENGTH);

    if (recv(client_sock, &message, sizeof(message), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno)); // ERROR HANDLING
        if (close(client_sock) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
        exit(1);
    }
    printf("Received message from client: %d %s\n", message.operation, message.buffer);

    if (message.operation == READ)
    {
        int fd = open(message.buffer, O_RDONLY);
        if (fd == -1)
        {
            fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
            err_code = FILE_NOT_READABLE;
            // return NULL;
        }

        if (send(client_sock, &err_code, sizeof(err_code), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
        }

        if (err_code == FILE_NOT_READABLE)
            return NULL;

        char buffer[1024];
        int bytes_read;

        while ((bytes_read = read(fd, buffer, 1024)) > 0)
        {
            buffer[bytes_read] = '\0';
            printf("Sending message to client: %s\n", buffer);
            if (send(client_sock, buffer, strlen(buffer), 0) < 0)
            {
                fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                if (close(client_sock) < 0)
                    fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
                exit(1);
            }
        }
        printf("\n");
        close(fd);
    }

    if (message.operation == WRITE)
    {
        int fd = open(message.buffer, O_WRONLY);
        if (fd == -1)
        {
            fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
            return NULL;
        }
        char buffer[1024];
        int bytes_read;
        while ((bytes_read = read(fd, buffer, 1024)) > 0)
        {
            buffer[bytes_read] = '\0';
            printf("%s", buffer);
        }
        printf("\n");
        close(fd);
    }
    if (message.operation == METADATA)
    {
        struct stat sb;
        metadata fileInfo;
        if (lstat(message.buffer, &sb) == -1)
        {
            fprintf(stderr, "\x1b[31mError acessing the metadata of %s.\n\n\x1b[0m", message.buffer); // ERROR HANDLING
            err_code = METADATA_INACESSIBLE;
        }
        if (send(client_sock, &err_code, sizeof(err_code), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
        }
        if (err_code != NO_ERROR)
        {
            return NULL;
        }

        fileInfo.inodeNumber = (uintmax_t)sb.st_ino;
        fileInfo.mode = (uintmax_t)sb.st_mode;
        fileInfo.linkCount = (uintmax_t)sb.st_nlink;
        fileInfo.uid = (uintmax_t)sb.st_uid;
        fileInfo.gid = (uintmax_t)sb.st_gid;
        fileInfo.preferredBlockSize = (intmax_t)sb.st_blksize;
        fileInfo.fileSize = (intmax_t)sb.st_size;
        fileInfo.blocksAllocated = (intmax_t)sb.st_blocks;
        fileInfo.lastStatusChange = sb.st_ctime;
        fileInfo.lastFileAccess = sb.st_atime;
        fileInfo.lastFileModification = sb.st_mtime;
        metadata *metadata = &fileInfo;
        printf("I-node number:                      %llu\n", metadata->inodeNumber);
        printf("Mode (permissions: octal):          %llo (octal)\n", metadata->mode);
        printf("Link count:                         %llu\n", metadata->linkCount);
        printf("Ownership:                          UID=%llu   GID=%llu\n", metadata->uid, metadata->gid);
        printf("File size:                          %lld bytes\n", metadata->fileSize);
        printf("Preferred I/O block size:           %lld bytes\n", metadata->preferredBlockSize);
        printf("Blocks allocated:                   %lld\n", metadata->blocksAllocated);
        printf("Last file access:                   %s", ctime(&metadata->lastFileAccess));
        printf("Last file modification:             %s", ctime(&metadata->lastFileModification));
        printf("Last status change:                 %s", ctime(&metadata->lastStatusChange));
        if (send(client_sock, &fileInfo, sizeof(fileInfo), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
        }
    }
    if (message.operation == COPY)
    {
    }
}

void *NMServerConnection(void *arg)
{
    int server_sock, nms_sock;
    struct sockaddr_in server_addr, nms_addr;
    socklen_t addr_size;
    // char buffer[MAX_PATH_LENGTH];
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
    port_for_naming_server_np = sin.sin_port;
    // printf("port extracted is %d\n", port_for_clients);
    sem_post(&portnmsNp_lock);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    printf("listening to respond to clients\n");

    while (1)
    {
        addr_size = sizeof(nms_addr);

        nms_sock = accept(server_sock, (struct sockaddr *)&nms_addr, &addr_size);
        if (nms_sock < 0)
        {
            fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
            if (close(server_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            // exit(1);
        }
        MessageClient2SS message;
        message.operation = 0;
        bzero(message.buffer, MAX_PATH_LENGTH);
        if (recv(nms_sock, &message, sizeof(message), 0) < 0)
        {
            fprintf(stderr, "[-]Receive error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(nms_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
            continue;
        }
        if (message.operation == 0)
            continue;
        printf("Received message from nm: %d %s\n", message.operation, message.buffer);
        printf("the message : %d\n", strlen(message.buffer));

        if (message.operation == CREATE && !message.isADirectory)
        {
            int err_code;
            int fd = open(message.buffer, O_CREAT | O_WRONLY, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
                err_code = FILE_UNABLE_TO_CREATE;
                // return NULL;
            }
            InsertTrie(message.buffer, ssTrie);
            err_code = NO_ERROR;

            if (send(nms_sock, &err_code, sizeof(err_code), 0) < 0)
            {
                fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                if (close(nms_sock) < 0)
                    fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
                // exit(1);
                continue;
            }

            if (err_code == FILE_UNABLE_TO_CREATE)
                return NULL;

            close(fd);
        }

        if (message.operation == CREATE && message.isADirectory)
        {
            int err_code;
            if (mkdir(message.buffer, 0777) == -1)
            {
                fprintf(stderr, "\x1b[31mCould not create %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
                err_code = DIRECTORY_UNABLE_TO_CREATE;
                // return NULL;
            }
            InsertTrie(message.buffer, ssTrie);
            err_code = NO_ERROR;

            if (send(nms_sock, &err_code, sizeof(err_code), 0) < 0)
            {
                fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                if (close(nms_sock) < 0)
                    fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
                exit(1);
            }

            if (err_code == DIRECTORY_UNABLE_TO_CREATE)
                return NULL;
        }

        if (message.operation == DELETE)
        {
            printf("In delete\n");
            int fd = open(message.buffer, O_RDONLY);
            if (fd == -1)
            {
                fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
                return NULL;
            }
            close(fd);
            if (remove(message.buffer) == 0)
            {
                printf("\x1b[32mDeleted %s successfully\n\n\x1b[0m", message.buffer);
                DeleteTrie(message.buffer, ssTrie);
            }
            else
                printf("\x1b[31mCould not delete %s\n\n\x1b[0m", message.buffer);
        }
        close(nms_sock);
    }
    if (close_flag == 1)
        return NULL;
}

void *clients_handler_worker(void *arg)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // char buffer[MAX_PATH_LENGTH];
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
    port_for_clients = sin.sin_port;
    // printf("port extracted is %d\n", port_for_clients);
    sem_post(&portc_lock);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    printf("listening to respond to clients\n");
    // listen for new clients and make a new thread if a client is found and do ss-client communication part in the worker function of that new thread
    while (1)
    {
        addr_size = sizeof(client_addr);
        // printf("1\n");
        // printf("%d\n",port_for_naming_server);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        // printf("2\n");
        if (client_sock < 0)
        {
            fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
            if (close(server_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
            exit(1);
        }
        pthread_t client_service_thread;
        pthread_create(&client_service_thread, NULL, CLientServerConnection, (void *)&client_sock);
    }
    return NULL;
}

void GetAccessiblePaths()
{
    printf("Enter 1 to make all paths accessible\n");
    printf("Enter 2 to make some paths accessible\n");
    int choice;
    scanf("%d", &choice);
    char current_directory[MAX_PATH_LENGTH];
    char TEMPBUFF[MAX_PATH_LENGTH];
    getcwd(current_directory, sizeof(current_directory));
    if (choice == 1)
    {
        printf("All paths are accessible\n");
        // get current directory storage server is in
        printf("Current directory: %s\n", current_directory);
        // append ss1 to cwdd
        // sprintf(current_directory, "%s/%s", current_directory, "ss1");
        char port_in_string[10];
        sprintf(port_in_string, "%d", port_for_clients);
        ssTrie = createNode(port_in_string);
        lookFor(current_directory, strlen(current_directory), ssTrie);
    }
    else if (choice == 2)
    {
        // make some paths accessible
        printf("Enter the number of directory paths you want to make accessible\n");
        int num_directory;
        scanf("%d", &num_directory);
        printf("Enter the paths for directories\n");
        char paths[num_directory][MAX_PATH_LENGTH];
        for (int i = 0; i < num_directory; i++)
        {
            scanf("%s", paths[i]);
            // handle paths being relative
            TEMPBUFF[0] = '\0';
            strcat(TEMPBUFF, current_directory);
            if (paths[i][0] != '/')
            {
                strcat(TEMPBUFF, "/");
            }
            strcat(TEMPBUFF, paths[i]);
            strcpy(paths[i], TEMPBUFF);
        }
        char port_in_string[10];
        sprintf(port_in_string, "%d", port_for_clients);
        ssTrie = createNode(port_in_string);
        for (int i = 0; i < num_directory; i++)
        {
            InsertTrie(paths[i] + strlen(current_directory), ssTrie);
            lookFor(paths[i], strlen(current_directory), ssTrie);
        }

        printf("Enter the number of files you want to make accessible\n");
        int num_files;
        scanf("%d", &num_files);
        printf("Enter the paths for files\n");
        char files[num_files][MAX_PATH_LENGTH];
        for (int i = 0; i < num_files; i++)
        {
            scanf("%s", files[i]);
        }
        for (int i = 0; i < num_files; i++)
        {
            InsertTrie(files[i], ssTrie);
        }

        // PrintTrie(ssTrie);
    }
    else
    {
        printf("Invalid choice\n"); // ERROR HANDLING
        // GetAccessiblePaths();
    }
}
int main()
{
    GetAccessiblePaths();
    pthread_t clients_handler, naming_server_informer, naming_server_responder;
    sem_init(&portc_lock, 0, 0);
    sem_init(&portnms_lock, 0, 0);
    sem_init(&portnmsNp_lock, 0, 0);
    pthread_create(&clients_handler, NULL, clients_handler_worker, NULL);
    pthread_create(&naming_server_responder, NULL, naming_server_responder_worker, NULL);
    pthread_create(&naming_server_informer, NULL, naming_server_informer_worker, NULL);
    while (1)
    {
    }
}