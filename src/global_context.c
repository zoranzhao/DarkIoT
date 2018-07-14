#include "global_context.h"
/*Queues used in gateway device*/
/*
thread_safe_queue** results_pool;
thread_safe_queue* ready_pool;
uint32_t* results_counter;
thread_safe_queue* registration_list;
*/

/*Queues used in edge device*/
/*
thread_safe_queue* task_queue;
thread_safe_queue* result_queue;
*/

static const char* addr_list[CLI_NUM] = EDGE_ADDR_LIST;

void init_context(device_ctxt* ctxt, uint32_t cli_id, uint32_t cli_num){
   uint32_t i;
   ctxt->results_pool = (thread_safe_queue**)malloc(sizeof(thread_safe_queue*)*cli_num);
   ctxt->results_counter = (uint32_t*)malloc(sizeof(uint32_t)*cli_num);
   for(i = 0; i < cli_num; i++){
      ctxt->results_pool[i] = new_queue(MAX_QUEUE_SIZE);
      ctxt->results_counter[i] = 0;
   } 
   ctxt->ready_pool = new_queue(MAX_QUEUE_SIZE); 
   ctxt->registration_list = new_queue(MAX_QUEUE_SIZE); 

   ctxt->task_queue = new_queue(MAX_QUEUE_SIZE);
   ctxt->result_queue = new_queue(MAX_QUEUE_SIZE); 

   ctxt->this_cli_id = cli_id;
   ctxt->total_cli_num = cli_num;

/*
   ctxt->addr_list[CLI_NUM];
   for(i = 0; i < cli_num; i++){
      
   }
*/ 
}
