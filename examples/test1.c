#include "darkiot.h"

int main(){

   thread_safe_queue* q = new_queue(10); 
   char testdata1[30]="testdata1!";
   char testdata2[30]="testdata2!";
   char testdata3[30]="testdata3!";
   char testdata4[30]="testdata4!";
   blob* b1 = new_blob_and_move_data(1, 30, (uint8_t*)testdata1);
   blob* b2 = new_blob_and_move_data(2, 30, (uint8_t*)testdata2);
   blob* b3 = new_blob_and_move_data(3, 30, (uint8_t*)testdata3);
   blob* b4 = new_blob_and_move_data(4, 30, (uint8_t*)testdata4);

   enqueue(q, b1);
   enqueue(q, b2);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   enqueue(q, b3);
   enqueue(q, b4);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   enqueue(q, b3);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   enqueue(q, b2);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   enqueue(q, b1);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   printf("%s\n", dequeue_and_return_blob(q)->data);
   return 0;

}
