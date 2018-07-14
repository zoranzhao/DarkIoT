#ifndef GLOBAL_CONTEXT_H
#define GLOBAL_CONTEXT_H
#include "thread_safe_queue.h"
#include "config.h"

typedef struct dev_ctxt {
   thread_safe_queue** results_pool;
   thread_safe_queue* ready_pool;
   uint32_t* results_counter;
   thread_safe_queue* registration_list;
   char* addr_list[CLI_NUM];
   uint32_t total_cli_num;

   thread_safe_queue* task_queue;
   thread_safe_queue* result_queue; 
   uint32_t this_cli_id;
} device_ctxt;


void init_context(device_ctxt* ctxt, uint32_t cli_num);

#endif

