#include "mthreads.h"

int main()
{
    pthread_t server;
    pthread_t clients[MAX_THREADS];
    int ID[MAX_THREADS] = {0};
    srand((unsigned int)time(NULL));

    init_free_pool(MAX_POOL);
    init_device_queue();

    if (pthread_create(&server, NULL, serverThread, NULL) != 0)
    {
        fprintf(stderr, "Error creating server thread.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_THREADS; i++)
    {
        ID[i] = i + 1;
        if (pthread_create(&clients[i], NULL, clientThread, &ID[i]) != 0)
        {
            fprintf(stderr, "Error creating client thread.\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int j = 0; j < MAX_REQUESTS; j++)
    {
        if (pthread_join(clients[j], NULL) != 0)
        {
            fprintf(stderr, "Error joining client thread.\n");
            exit(EXIT_FAILURE);
        };
    }

    puts("after all clients join.");

    if (pthread_join(server, NULL) != 0)
    {
        fprintf(stderr, "Error joining server thread.\n");
        exit(EXIT_FAILURE);
    }

    puts("after join server");

    free_pool_destroy();
    device_queue_destroy();

    puts("Finished");
    return 0;
}

void *serverThread(void *arg)
{
    while (active_clients > 0 || device_head)
    {
        buff_node_ptr curr_device_node = device_dequeue();
        printf("Server received: %s\n", curr_device_node->buff);
        ret_node(curr_device_node);
    }
    return NULL;
}

void *clientThread(void *arg)
{
    int *client_num = (int *)arg;
    for (int request_num = 0; request_num <= MAX_REQUESTS; ++request_num)
    {
        usleep(rand() % 101 * 1000);

        buff_node_ptr curr_free_pool_node = get_node();
        sprintf(curr_free_pool_node->buff, "client-%04d request-%03d", *client_num, request_num);
        device_enqueue(curr_free_pool_node);
    }
    count_active_client();
}
