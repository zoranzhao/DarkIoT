#include "thread_safe_queue.h"

blob* new_blob_and_move_data(int32_t id, uint32_t size, uint8_t* data)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = data;
   temp->size = size;
   temp->id = id;
   return temp; 
}

blob* new_blob_and_alloc_data(int32_t id, uint32_t size)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = (uint8_t*)malloc(sizeof(uint8_t));
   temp->size = size;
   temp->id = id;
   return temp; 
}

blob* new_empty_blob(int32_t id)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = NULL;
   temp->size = 0;
   temp->id = id;
   return temp; 
}

void free_blob(blob* temp){
   if(temp->data != NULL){
      free(temp->data);
   }
   free(temp);
}

queue_node* new_node_and_copy_item(blob* item)
{
   queue_node *temp = (queue_node*)malloc(sizeof(queue_node));
   temp->item = item;
   temp->next = NULL;
   return temp; 
}

queue_node* new_empty_node()
{
   queue_node *temp = (queue_node*)malloc(sizeof(queue_node));
   temp->item = NULL;
   temp->next = NULL;
   return temp; 
}

thread_safe_queue *new_queue(uint32_t capacity)
{
    thread_safe_queue *q = (thread_safe_queue*)malloc(sizeof(thread_safe_queue));
    q->head = q->tail = NULL;
    q->capacity = capacity;
    return q;
}

void enqueue(thread_safe_queue *q, blob* item)
{
    queue_node *temp = new_node_and_copy_item(item);
    if (q->tail == NULL)
    {
       q->head = q->tail = temp;
       return;
    }
    q->tail->next = temp;
    q->tail = temp;
}

blob* dequeue_and_return_blob(thread_safe_queue *q)
{
    if (q->head == NULL)
       return NULL;
 
    queue_node *temp = q->head;
    q->head = q->head->next;
    if (q->head == NULL)
       q->tail = NULL;

    blob* item = temp->item; 
    free(temp);
    return item;
}

queue_node* dequeue_and_return_node(thread_safe_queue *q)
{
    if (q->head == NULL)
       return NULL;
 
    queue_node *temp = q->head;
    q->head = q->head->next;
    if (q->head == NULL)
       q->tail = NULL;
    return temp;
}

