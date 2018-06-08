#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H
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

typedef struct ts_queue {
   queue_node* head, *tail;
} thread_safe_queue;

#endif
