#ifndef MTHREADS_H
#define MTHREADS_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_THREADS 1000
#define MAX_POOL 20
#define MAX_REQUESTS 20

typedef struct buff_node_type
{
    char buff[51];
    struct buff_node_type *next, *prev;
} buff_node, *buff_node_ptr;

extern pthread_mutex_t mutex;
extern pthread_cond_t cond_server;
extern pthread_cond_t cond_client;
extern buff_node_ptr device_head, device_tail;
extern buff_node_ptr free_pool_head;
extern int active_clients;

void init_free_pool(int count);
buff_node_ptr get_node();
void ret_node(buff_node_ptr p);
int count_free_pool_nodes();
void free_pool_destroy();
void init_device_queue();
void device_enqueue(buff_node_ptr p);
buff_node_ptr device_dequeue();
int count_device_queue_nodes();
void device_queue_destroy();
void *serverThread(void *arg);
void *clientThread(void *arg);
void count_active_client();

#endif