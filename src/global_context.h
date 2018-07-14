#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H
#include "thread_safe_queue.h"
#include "config.h"
#include <string.h>
#define ADDR_LEN 64 

typedef struct dev_ctxt {
   thread_safe_queue** results_pool;
   thread_safe_queue* ready_pool;
   uint32_t* results_counter;
   thread_safe_queue* registration_list;
   char addr_list[CLI_NUM][ADDR_LEN];
   uint32_t total_cli_num;

   thread_safe_queue* task_queue;
   thread_safe_queue* result_queue; 
   uint32_t this_cli_id;

   uint32_t batch_size;/*Number of tasks to merge*/
   void *model;/*pointers to execution model*/
} device_ctxt;

device_ctxt* init_context(uint32_t cli_id, uint32_t cli_num);

#endif

