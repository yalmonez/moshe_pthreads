#include "mthreads.h"

void printdevicequeue();

int main(int arg, char **argv){
    init_device_queue();
    init_free_pool(MAX_REQUESTS);
    buff_node_ptr temp_node = free_pool_head;
    int i = 0;

    while(temp_node->next){
        sprintf(temp_node->buff, "test_%d", i++);
        temp_node = temp_node->next;
    }
    
    sprintf(temp_node->buff, "test_%d", i);
    temp_node = get_node();
    printf("free_pool: %s\n", temp_node->buff);
    device_enqueue(temp_node);
    for (int i = 0; i < 7; i++) {
        temp_node = get_node();
        device_enqueue(temp_node);
    }
    printf("device_tail: %s\n", device_tail->buff);
    printf("device_head: %s\n", device_head->buff);
    printf("free_pool_head: %s\n", free_pool_head->buff);
    printf("count of free_pool: %d\n", count_free_pool_nodes());
    printf("count of device_queue: %d\n", count_device_queue_nodes());
    device_dequeue();
    printf("count of free_pool: %d\n", count_free_pool_nodes());
    printf("count of device_queue: %d\n", count_device_queue_nodes());
    return 0;
}

// void printdevicequeue() {
//     for (int i = count_free_pool_nodes(); i < 0; i--) {
//         printf("current_node");
//     }
// }