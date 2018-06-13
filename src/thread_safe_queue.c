#include "thread_safe_queue.h"

/*
Programming API for data blobs
*/
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

/*
Programming API for thread safe queue.
*/
static queue_node* new_node_and_copy_item(blob* item)
{
   queue_node *temp = (queue_node*)malloc(sizeof(queue_node));
   temp->item = item;
   temp->next = NULL;
   return temp; 
}

thread_safe_queue *new_queue(uint32_t capacity)
{
   thread_safe_queue *q = (thread_safe_queue*)malloc(sizeof(thread_safe_queue));
   q->head = q->tail = NULL;
   q->capacity = capacity;
   q->number_of_node = 0;
   sys_sem_new(&(q->not_empty), 0);
   sys_sem_new(&(q->not_full), 0);
   sys_sem_new(&(q->mutex), 1);
   q->wait_send = 0;
   return q;
}

void enqueue(thread_safe_queue *queue, blob* item)
{
   uint8_t first;
   queue_node *temp = new_node_and_copy_item(item);
   sys_arch_sem_wait(&(queue->mutex), 0);
   while ((queue->number_of_node) >= (queue->capacity)) {
      queue->wait_send++;
      sys_sem_signal(&queue->mutex);
      sys_arch_sem_wait(&queue->not_full, 0);
      sys_arch_sem_wait(&queue->mutex, 0);
      queue->wait_send--;
   }

   if (queue->tail == NULL){
      queue->head = queue->tail = temp;
      first = 1;
   }else{
      queue->tail->next = temp;
      queue->tail = temp;
      first = 0;
   }
   queue->number_of_node++;

   if (first) {
      sys_sem_signal(&queue->not_empty);
   }

   sys_sem_signal(&queue->mutex);

}

blob* dequeue(thread_safe_queue *queue)
{
   sys_arch_sem_wait(&queue->mutex, 0);
   while (queue->head == NULL) {
      sys_sem_signal(&queue->mutex);
      /* We block while waiting for a mail to arrive in the mailbox. */
      sys_arch_sem_wait(&queue->not_empty, 0);
      sys_arch_sem_wait(&queue->mutex, 0);
   } 

   queue_node *temp = queue->head;
   queue->head = queue->head->next;
   if (queue->head == NULL)
      queue->tail = NULL;
   blob* item = temp->item; 
   free(temp);
   queue->number_of_node--;

   if (queue->wait_send) {
      sys_sem_signal(&queue->not_full);
   }

   sys_sem_signal(&queue->mutex);

   return item;
}


blob* try_dequeue(thread_safe_queue *queue)
{
   sys_arch_sem_wait(&queue->mutex, 0);
   while (queue->head == NULL) {
      sys_sem_signal(&queue->mutex);
      return NULL;
   } 

   queue_node *temp = queue->head;
   queue->head = queue->head->next;
   if (queue->head == NULL)
      queue->tail = NULL;
   blob* item = temp->item; 
   free(temp);
   queue->number_of_node--;

   if (queue->wait_send) {
      sys_sem_signal(&queue->not_full);
   }

   sys_sem_signal(&queue->mutex);

   return item;
}
void free_queue(thread_safe_queue *queue)
{
  if (queue != NULL) {
    sys_arch_sem_wait(&queue->mutex, 0);
    sys_sem_free(&queue->not_empty);
    sys_sem_free(&queue->not_full);
    sys_sem_free(&queue->mutex);
    queue->not_empty = queue->not_full = queue->mutex = NULL;
    free(queue);
  }
}


