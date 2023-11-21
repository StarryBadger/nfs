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
int write_count = 0;

void *NMServerConnection(void *arg);

void *naming_server_responder_worker(void *arg)
{
    // printf("here i am\n");
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    // char buffer[PATH_MAX];
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
    char buffer[PATH_MAX];
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

    // bzero(buffer, PATH_MAX);
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
    close(ss_sock);
    return NULL;
}

void Read_ss(int *err_code, int client_sock, MessageClient2SS message, FILE *file, int isClient)
{
    if (file == NULL)
    {
        fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer);
        *err_code = FILE_NOT_READABLE;
    }

    if (isClient)
    {
        if (send(client_sock, err_code, sizeof(*err_code), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
            close(client_sock);
            *err_code = NETWORK_ERROR;
            return;
        }

        if (*err_code == FILE_NOT_READABLE)
        {
            return;
        }
    }

    // FILE* this = fopen("this.txt", "w");
    char buffer[SEND_SIZE];
    bzero(buffer, SEND_SIZE);
    size_t bytesRead = 0;
    MessageFormat message_read;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        // fwrite(buffer, 1, bytesRead, this);
        // printf("Sending message to client: %s\n", buffer);
        bzero(message_read.msg, PATH_MAX);
        strcpy(message_read.msg, buffer);
        message_read.bytesToRead = bytesRead;
        if (send(client_sock, &message_read, sizeof(message_read), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno));
            close(client_sock);
            *err_code = NETWORK_ERROR;
            return;
        }
        bzero(buffer, SEND_SIZE);
        bytesRead = 0;
    }

    // closeSocket(client_sock);
    // printf("\n");
    // fclose(file);
}

int Write_ss(int *err_code, int client_sock, MessageClient2SS message, FILE *fd, int isCLient)
{
    if (fd == -1)
    {
        fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
        *err_code = FILE_NOT_WRITABLE;
    }
    char buffer[PATH_MAX];
    int bytes_read;

    if (isCLient)
    {
        if (send(client_sock, err_code, sizeof(*err_code), 0) < 0)
        {
            fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
        }

        if (*err_code == FILE_NOT_WRITABLE)
            return NULL;
    }

    int bytesRead = 0;

    // printf("Received message from client: %s\n", message.msg);
    // printf("size of message: %d\n", sizeof(message.msg));
    // if (write(fd, message.msg, strlen(message.msg)) < 0)

    // FILE *th = fopen("this.txt", "a");
    if(!isCLient)
    {
        if (fwrite(message.msg, 1, message.bytesToRead, fd) < 0)
        {
            fprintf(stderr, "[-]Write error: %s\n", strerror(errno)); // ERROR HANDLING
            if (close(client_sock) < 0)
                fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
        }
        write_count++;
        printf("write count: %d\n", write_count);
    }
    MessageFormat message2;
    bzero(message2.msg, PATH_MAX);

    // while ((bytesRead = recv(client_sock, &message2, sizeof(message2), 0)) > 0)
    // {
    //     // fwrite(message2.msg, 1, bytesRead, th);
    //     if(fwrite(message2.msg, 1, bytesRead, fd) < 0)
    //     {
    //         fprintf(stderr, "[-]Write error: %s\n", strerror(errno)); // ERROR HANDLING
    //         if (close(client_sock) < 0)
    //             fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
    //         // exit(1);
    //     }
    //     bzero(message2.msg, PATH_MAX);
    // }
    return bytes_read;
}

void *CLientServerConnection(void *arg)
{
    int client_sock = *(int *)arg;
    int err_code = NO_ERROR;
    MessageClient2SS message;
    bzero(message.buffer, PATH_MAX);

    if (recv(client_sock, &message, sizeof(message), 0) < 0)
    {
        fprintf(stderr, "[-]Receive error: %s\n", strerror(errno)); // ERROR HANDLING
        if (close(client_sock) < 0)
            fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
        exit(1);
    }
    // printf("Received message from client: %d %s\n", message.operation, message.buffer);

    if (message.operation == READ)
    {
        // int fd = open(message.buffer, O_RDONLY);
        // Read_ss(&err_code, client_sock, message,fd,1);

        FILE *file = fopen(message.buffer, "r");
        Read_ss(&err_code, client_sock, message, file, 1);

        closeSocket(client_sock);
        // printf("\n");
        // close(fd);
        fclose(file);
    }

    if (message.operation == WRITE)
    {
        // int fd = open(message.buffer, O_WRONLY | O_TRUNC);
        FILE *fd = fopen(message.buffer, "w");
        int b_read = Write_ss(&err_code, client_sock, message, fd, 1);
        if (b_read < 0)
            closeSocket(client_sock);
        // printf("\n");
        fclose(fd);
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
    // char buffer[PATH_MAX];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        fprintf(stderr, "[-]Socket creation error: %s\n", strerror(errno));
        // exit(1);
    }
    printf("[+]TCP server socket created.\n");

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
    printf("port extracted is %d\n", port_for_clients);
    sem_post(&portnmsNp_lock);
    if (listen(server_sock, 5) < 0)
    {
        fprintf(stderr, "[-]Storage server got disconnected from Naming Server %s\n", strerror(errno));
        close(server_sock);
        exit(1);
    }
    printf("listening to respond to naming server\n");

    // addr_size = sizeof(nms_addr);

    // nms_sock = accept(server_sock, (struct sockaddr *)&nms_addr, &addr_size);
    // if (nms_sock < 0)
    // {
    //     fprintf(stderr, "[-]Accept error: %s\n", strerror(errno));
    //     if (close(server_sock) < 0)
    //         fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno));
    //     // exit(1);
    // }
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
        printf("Accepted\n");
        MessageClient2SS message;
        message.operation = 0;
        bzero(message.buffer, PATH_MAX);
        bzero(message.msg, PATH_MAX);
        // FILE *this = fopen("this.txt", "w");
        int bytes_read;
        if ((bytes_read = recv(nms_sock, &message, sizeof(message), 0)) < 0)
        {
            fprintf(stderr, "[-]Receive error: %s\n", strerror(errno)); // ERROR HANDLING
            // if (close(nms_sock) < 0)
            //     fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
            // exit(1);
            continue;
        }
        printf("doen recv00;\n");
        // fwrite(message.msg, 1, strlen(message.msg), this);
        // fclose(this);
        if (message.operation == 0 || bytes_read == 0)
            continue;
        // printf("the message : %d\n", strlen(message.buffer));

        if (message.operation == CREATE && !message.isADirectory)
        {
            printf("one\n");
            int err_code;
            int fd = open(message.buffer, O_CREAT | O_WRONLY, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
                err_code = FILE_UNABLE_TO_CREATE;
                // return NULL;
            }
            // InsertTrie(message.buffer, ssTrie); //SHREYANSH
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
            printf("two\n");
            int err_code;
            if (mkdir(message.buffer, 0777) == -1)
            {
                fprintf(stderr, "\x1b[31mCould not create %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
                err_code = DIRECTORY_UNABLE_TO_CREATE;
                // return NULL;
            }
            // InsertTrie(message.buffer, ssTrie); // SHREYANSH
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
            // int fd = open(message.buffer, O_RDONLY);
            // if (fd == -1)
            // {
            //     fprintf(stderr, "\x1b[31mCould not open %s. Permission denied\n\n\x1b[0m", message.buffer); // ERROR HANDLING
            //     return NULL;
            // }
            // close(fd);
            int err_code = NO_ERROR;
            printf("to be deleted: %s\n", message.buffer);
            if (remove(message.buffer) == 0)
            {
                printf("\x1b[32mDeleted %s successfully\n\n\x1b[0m", message.buffer);
                // DeleteTrie(message.buffer, ssTrie);
            }
            else
            {
                printf("\x1b[31mCould not delete %s\n\n\x1b[0m", message.buffer);
                err_code = UNABLE_TO_DELETE;
            }

            if (send(nms_sock, &err_code, sizeof(err_code), 0) < 0)
            {
                fprintf(stderr, "[-]Send time error: %s\n", strerror(errno)); // ERROR HANDLING
                if (close(nms_sock) < 0)
                    fprintf(stderr, "[-]Error closing socket: %s\n", strerror(errno)); // ERROR HANDLING
                exit(1);
            }
        }

        int err_code = NO_ERROR;

        if (message.operation == READ)
        {
            // int fd = open(message.buffer, O_RDONLY);
            FILE *file = fopen(message.buffer, "r");
            Read_ss(&err_code, nms_sock, message, file, 0);
            fclose(file);
        }

        if (message.operation == WRITE)
        {
            // int fd = open(message.buffer, O_WRONLY | O_TRUNC);
            FILE *fd = fopen(message.buffer, "a");
            Write_ss(&err_code, nms_sock, message, fd, 0);
            fclose(fd);
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
    // char buffer[PATH_MAX];
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
    char current_directory[PATH_MAX];
    char TEMPBUFF[PATH_MAX];
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
        ssTrie = createNode(port_in_string, 0, 1);
        lookFor(current_directory, strlen(current_directory), ssTrie);
    }
    else if (choice == 2)
    {
        // make some paths accessible
        printf("Enter the number of directory paths you want to make accessible\n");
        int num_directory;
        scanf("%d", &num_directory);
        printf("Enter the paths for directories\n");
        char paths[num_directory][PATH_MAX];
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
        ssTrie = createNode(port_in_string, 0, 1);
        for (int i = 0; i < num_directory; i++)
        {
            InsertTrie(paths[i] + strlen(current_directory), ssTrie, 0, 1);
            lookFor(paths[i], strlen(current_directory), ssTrie);
        }

        printf("Enter the number of files you want to make accessible\n");
        int num_files;
        scanf("%d", &num_files);
        printf("Enter the paths for files\n");
        char files[num_files][PATH_MAX];
        for (int i = 0; i < num_files; i++)
        {
            scanf("%s", files[i]);
        }
        for (int i = 0; i < num_files; i++)
        {
            InsertTrie(files[i], ssTrie, 1, 1);
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