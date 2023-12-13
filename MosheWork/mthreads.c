#include "mthreads.h"

/*TODO
buff_node_ptr init_free_pool(int count);       [V]
buff_node_ptr init_device_queue();             [V]
buff_node_ptr get_node();                      [V]
void ret_node(buff_node_ptr p);                [V]
void device_enqueue(buff_node_ptr p);          [V]
buff_node_ptr device_dequeue();                [V]
int count_free_pool_nodes();                   [V]
int count_device_queue_nodes();                [V]
void free_pool_destroy();                      [V]
void device_queue_destroy();                   [V]
void count_active_client();                    [V]
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_server = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_client = PTHREAD_COND_INITIALIZER;

buff_node_ptr device_head, device_tail = NULL;
buff_node_ptr free_pool_head = NULL;

int active_clients = MAX_THREADS;

int count_free_pool_nodes()
{
    int count = 0;
    buff_node_ptr node = free_pool_head;
    while (node)
    {
        count++;
        node = node->next;
    }
    return count;
}

int count_device_queue_nodes()
{
    int count = 0;
    buff_node_ptr node = device_head;
    while (node)
    {
        count++;
        node = node->next;
    }
    return count;
}

void init_free_pool(int count)
{
    buff_node_ptr free_pool_node = (buff_node_ptr)malloc(sizeof(buff_node));
    if (!free_pool_node)
    {
        fprintf(stderr, "free_pool_node memory allocation failed");
        exit(EXIT_FAILURE);
    }
    free_pool_head = free_pool_node;
    for (int i = 0; i < count; i++)
    {
        free_pool_node->prev = NULL;
        free_pool_node->next = (buff_node_ptr)malloc(sizeof(buff_node));
        if (!free_pool_node->next)
        {
            fprintf(stderr, "free_pool_node->next memory allocation failed");
            exit(EXIT_FAILURE);
        }
        free_pool_node = free_pool_node->next;
    }
    free_pool_node->prev = NULL;
    free_pool_node->next = NULL;
}

buff_node_ptr get_node()
{
    pthread_mutex_lock(&mutex);

    while (!free_pool_head)
    {
        pthread_cond_wait(&cond_client, &mutex);
    }

    buff_node_ptr node = free_pool_head;
    free_pool_head = node->next;

    pthread_mutex_unlock(&mutex);

    return node;
}

void init_device_queue()
{
    device_head = NULL;
    device_tail = device_head;
}

void device_enqueue(buff_node_ptr p)
{
    pthread_mutex_lock(&mutex);

    p->next = NULL;

    if (!device_head)
    {
        device_head = p;
        device_tail = p;
        p->prev = NULL;
    }
    else
    {
        device_tail->next = p;
        p->prev = device_tail;
        device_tail = p;
    }

    pthread_cond_signal(&cond_server);
    pthread_mutex_unlock(&mutex);
}

void ret_node(buff_node_ptr p)
{
    pthread_mutex_lock(&mutex);

    buff_node_ptr curr_node = free_pool_head;
    while (curr_node->next)
    {
        curr_node = curr_node->next;
    }
    curr_node->next = p;
    p->next = NULL;

    pthread_cond_signal(&cond_client);
    pthread_mutex_unlock(&mutex);
}

buff_node_ptr device_dequeue()
{
    pthread_mutex_lock(&mutex);

    while (!device_head)
    {
        pthread_cond_wait(&cond_server, &mutex);
    }

    if (!device_head)
        return NULL;

    buff_node_ptr curr_node = device_head;
    device_head = device_head->next;

    if (device_head)
    {
        device_head->prev = NULL;
    }
    else
    {
        device_tail = NULL;
    }

    curr_node->next, curr_node->prev = NULL;
    pthread_mutex_unlock(&mutex);
    return curr_node;
}

void free_pool_destroy()
{
    while (free_pool_head != NULL)
    {
        buff_node_ptr node = get_node();
        free(node);
    }

    pthread_cond_destroy(&cond_client);
}

void device_queue_destroy()
{
    while (free_pool_head != NULL)
    {
        buff_node_ptr node = device_dequeue();
        free(node);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_server);
}

void count_active_client()
{
    pthread_mutex_lock(&mutex);
    active_clients--;
    pthread_mutex_unlock(&mutex);
}