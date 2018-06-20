#ifndef EXEC_CTRL_H
#define EXEC_CTRL_H
#include "data_blob.h"
#include "thread_util.h"
#include "thread_safe_queue.h"
#include "network_util.h"

void exec_barrier(int portno, ctrl_proto proto);
void exec_start_gateway(int portno, ctrl_proto proto);

#endif