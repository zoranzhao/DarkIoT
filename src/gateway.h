#ifndef GATEWAY_H
#define GATEWAY_H
#include "config.h"
#include "exec_ctrl.h"

void init_gateway();
void collect_result_thread(void *arg);
void merge_result_thread(void *arg);
void work_stealing_thread(void *arg);

#endif
