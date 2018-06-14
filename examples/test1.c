#include "darkiot.h"
#include <string.h>

static thread_safe_queue* q; 

void consumer(void *arg){
   blob* tmp;
   while(1){
      tmp = dequeue(q);
      printf("%s\n", tmp->data);
      printf("number_of_node %d\n", q->number_of_node);
      free_blob(tmp);
   }
}

void producer(void *arg){
   char testdata1[30]="testdata1!";
   char testdata2[30]="testdata2!";
   char testdata3[30]="testdata3!";
   char testdata4[30]="testdata4!";
   blob* b1 = new_blob_and_copy_data(1, 30, (uint8_t*)testdata1);
   blob* b2 = new_blob_and_move_data(2, 30, (uint8_t*)testdata2);
   blob* b3 = new_blob_and_move_data(3, 30, (uint8_t*)testdata3);
   blob* b4 = new_blob_and_copy_data(4, 30, (uint8_t*)testdata4);
   blob* b5 = new_blob_and_alloc_data(5, 30);
   strcpy((char*)b5->data, "testdata5!");

   enqueue(q, b1);
   enqueue(q, b2);
   enqueue(q, b3);
   enqueue(q, b4);

   enqueue(q, b3);
   enqueue(q, b2);
   enqueue(q, b1);
   enqueue(q, b5);

   free_blob(b1);
   free_blob(b2);
   free_blob(b3);
   free_blob(b4);
   free_blob(b5);
}



int main(){
   q = new_queue(20); 
   sys_thread_t t1 = sys_thread_new("consumer", consumer, NULL, 0, 0);
   sys_thread_t t2 = sys_thread_new("producer", producer, NULL, 0, 0);
   sys_thread_join(t1);
   sys_thread_join(t2);
   return 0;

}
