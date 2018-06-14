#include "darkiot.h"
#include <string.h>

static thread_safe_queue* q; 

int main(){
   q = new_queue(10); 
   char testdata1[30]="testdata1!";
   char testdata2[30]="testdata2!";
   char testdata3[30]="testdata3!";
   char testdata4[30]="testdata4!";
   blob* b1 = new_blob_and_move_data(1, 30, (uint8_t*)testdata1);
   blob* b2 = new_blob_and_move_data(2, 30, (uint8_t*)testdata2);
   blob* b3 = new_blob_and_move_data(3, 30, (uint8_t*)testdata3);
   blob* b4 = new_blob_and_move_data(4, 30, (uint8_t*)testdata4);
   blob* b5 = new_blob_and_alloc_data(5, 30);
   strcpy((char*)b5->data, "testdata5!");

   enqueue(q, b1);
   enqueue(q, b2);
   printf("%s\n", dequeue(q)->data);
   enqueue(q, b3);
   enqueue(q, b4);
   printf("%s\n", dequeue(q)->data);
   enqueue(q, b3);
   printf("%s\n", dequeue(q)->data);
   enqueue(q, b2);
   printf("%s\n", dequeue(q)->data);
   enqueue(q, b1);
   printf("%s\n", dequeue(q)->data);
   printf("%s\n", dequeue(q)->data);
   printf("%s\n", dequeue(q)->data);
   enqueue(q, b5);
   printf("%s\n", dequeue(q)->data);
   printf("try_dequeue %d\n", (try_dequeue(q)==NULL));

   free_blob(b1);
   free_blob(b2);
   free_blob(b3);
   free_blob(b4);
   free_blob(b5);

   return 0;

}
