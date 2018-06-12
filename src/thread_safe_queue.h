#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H
#include "thread_util.h"
#include <stdint.h>
#include <stdlib.h>

typedef struct ts_blob {
   uint8_t *data;
   uint32_t size;
   int32_t id;
} blob;

typedef struct ts_node {
   blob* item;
   struct ts_node* next;
} queue_node;

/*Set a capacity to consider the limited memory space in IoT devices*/
typedef struct ts_queue {
   uint32_t capacity;
   queue_node* head, *tail;
   sys_sem_t not_empty;
   sys_sem_t not_full;
   sys_sem_t mutex;
} thread_safe_queue;

blob* new_blob_and_move_data(int32_t id, uint32_t size, uint8_t* data);
blob* new_blob_and_alloc_data(int32_t id, uint32_t size);
blob* new_empty_blob(int32_t id);
void free_blob(blob* temp);
queue_node* new_node_and_copy_item(blob* item);
queue_node* new_empty_node();

thread_safe_queue *new_queue(uint32_t capacity);
void enqueue(thread_safe_queue *q, blob* item);
queue_node* dequeue_and_return_node(thread_safe_queue *q);
blob* dequeue_and_return_blob(thread_safe_queue *q);


#endif
