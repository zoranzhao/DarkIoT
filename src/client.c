#include "client.h"

static thread_safe_queue* task_queue;
static thread_safe_queue* result_queue;

void init_client(){
   task_queue = new_queue(MAX_QUEUE_SIZE);
   result_queue = new_queue(MAX_QUEUE_SIZE); 
}

static void register_client(){
   char request_type[20] = "register_gateway";
   service_conn* conn = connect_service(TCP, GATEWAY, WORK_STEAL_PORT);
   send_request(request_type, 20, conn);
   close_service_connection(conn);
}

static void cancel_client(){
   char request_type[20] = "cancel_gateway";
   service_conn* conn = connect_service(TCP, GATEWAY, WORK_STEAL_PORT);
   send_request(request_type, 20, conn);
   close_service_connection(conn);
}

static void process_task(blob* temp){
   blob* result;
   char data[20] = "output_data";
   result = new_blob_and_copy_data(temp->id, 20, (uint8_t*)data);
   enqueue(result_queue, result); 
   free_blob(result);
}

void steal_and_process_thread(void *arg){
   /*Check gateway for possible stealing victims*/
   service_conn* conn;
   blob* temp;
#if DEBUG_FLAG
   uint32_t task_counter = 0;   
#endif
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
      process_task(temp);
      free_blob(temp);
#if DEBUG_FLAG
      task_counter ++;   
      printf("Processed stolen task number is%d\n", task_counter);
#endif
   }
}

void generate_and_process_thread(void *arg){
   uint32_t task;
   blob* temp;
   char data[20] = "input_data";
#if DEBUG_FLAG
   uint32_t task_counter = 0;   
#endif
   register_client();
   for(task = 0; task < BATCH_SIZE; task ++){
      temp = new_blob_and_copy_data((int32_t)task, 20, (uint8_t*)data);
      enqueue(task_queue, temp);
      free_blob(temp);
   }
   while(1){
      temp = try_dequeue(task_queue);
      if(temp == NULL) break;
      process_task(temp);
      free_blob(temp);
#if DEBUG_FLAG
      task_counter ++;   
#endif
   }
#if DEBUG_FLAG
   printf("Locally processed task number is%d\n", task_counter);
#endif
   cancel_client();
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



