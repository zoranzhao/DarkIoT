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
      
      conn = connect_service(TCP, temp->data, WORK_STEAL_PORT);
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


void generate_task_thread(void *arg);
void send_result_thread(void *arg);
void process_task_thread(void *arg);
void serve_stealing_thread(void *arg);
