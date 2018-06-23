#ifndef CLIENT_H
#define CLIENT_H
#include "config.h"
#include "exec_ctrl.h"

void init_client();
void steal_task_thread(void *arg);
void generate_task_thread(void *arg);
void send_result_thread(void *arg);
void process_task_thread(void *arg);
void serve_stealing_thread(void *arg);

#endif
