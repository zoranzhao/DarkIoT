#include "client.h"

static thread_safe_queue* task_queue;
static thread_safe_queue* result_queue;

void init_client(){
   task_queue = new_queue(MAX_QUEUE_SIZE);
   result_queue = new_queue(MAX_QUEUE_SIZE); 
}

void steal_task_thread(void *arg){
   /*Check gateway for possible stealing victims*/
   service_conn* conn;
   blob* temp;
   while(1){
      conn = connect_service(TCP, GATEWAY, WORK_STEAL_PORT);
      send_request("steal_gateway", 20, conn);
      temp = recv_data(conn);
      close_service_connection(conn);
      if(temp->id == -1){
         free_blob(temp);
         sys_sleep(100);
         continue;
      }
      
      conn = connect_service(TCP, (const char *)temp->data, WORK_STEAL_PORT);
      send_request("steal_client", 20, conn);
      free_blob(temp);
      temp = recv_data(conn);
      close_service_connection(conn);
      if(temp->id == -1){
         free_blob(temp);
         sys_sleep(100);
         continue;
      }
      
      enqueue(task_queue, temp);
      free_blob(temp);
   }

}

void generate_task_thread(void *arg){
   uint32_t task;
   blob* temp;
   char data[20] = "input_data";
   for(task = 0; task < BATCH_SIZE; task ++){
      temp = new_blob_and_copy_data((int32_t)task, 20, (uint8_t*)data);
      enqueue(task_queue, temp);
      free_blob(temp);
   }
}

void send_result_thread(void *arg){
   service_conn* conn;
   blob* temp;
   while(1){
      conn = connect_service(TCP, GATEWAY, RESULT_COLLECT_PORT);
      send_request("result_gateway", 20, conn);
      temp = dequeue(result_queue);
      send_data(temp, conn);
      free_blob(temp);
      close_service_connection(conn);
   }
}

void process_task_thread(void *arg){
   uint32_t task = 0;
   blob* temp;
   blob* result;
   char data[20] = "output_data";
   while(1){
      temp = dequeue(result_queue);
      free_blob(temp);
      result = new_blob_and_copy_data((int32_t)task, 20, (uint8_t*)data);
      task++;
      enqueue(task_queue, result);
   }
}

void* steal_client(void* srv_conn){
   printf("steal_client ... ... \n");
   service_conn *conn = (service_conn *)srv_conn;
   blob* temp = try_dequeue(task_queue);
   if(temp == NULL){
      char data[20]="empty";
      temp = new_blob_and_copy_data(-1, 20, (uint8_t*)data);
   }
   send_data(temp, conn);
   free_blob(temp);
   return NULL;
}


void serve_stealing_thread(void *arg){
   const char* request_types[]={"steal_client"};
   void* (*handlers[])(void*) = {steal_client};
   int wst_service = service_init(WORK_STEAL_PORT, TCP);
   start_service(wst_service, TCP, request_types, 1, handlers);
   close_service(wst_service);
}



