#ifndef __NMS_H
#define __NMS_H
#include "trie.h"

struct ss_list
{
    int index;
    int ssToc_port;
    int ssTonms_port;
    int ssTonmnp_port;
    int ssTonmred_port;
    struct ss_list *my_red1_loc;
    struct ss_list *my_red2_loc;
    struct ss_list *prev_red1;
    struct ss_list *prev_red2;
    struct ss_list *next;
    struct ss_list *prev;
    TrieNode *root;
};

struct storage_servers_node
{
    struct ss_list *head;
    int total_servers;
};

int lessgoRec_again(int port, char **path_line, int index, TrieNode *node, char *path, int flag,int port_flag);
int lessgoRec(int port, int port2, char **path_line, int index, TrieNode *node, int initial_index, char *dest_path, int level_flag,int port_flag);
void *ss_is_alive_worker(void *arg);
int CopyPath2Path(char *src_path, char *dest_path);
void *client_handler(void *arg);
void *client_connection_worker(void *arg);
void CreateRedundancy(struct ss_list *source, struct ss_list *destination, int rednum_flag);
void deleteRedundancy(struct ss_list *dest, int red_flag);
void HandleRedundancy(struct ss_list *deleted_ss);
char *pathString(char **path_line, int size, int start);
void ss_is_alive_checker();
void *ss_port_worker(void *arg);
void RemoveSS(int index);
void InsertNewSS(int ssTocPortNo, int ssTonmsPortNo, int ssToNMmpport, int ssToNmRed_port, TrieNode *ssRoot);
void init_storage_servers();
int searchPortForNMS(char *buffer);
int searchPortForClient(char *buffer, int operation);
int initialize_nms_as_client(int port);

#endif