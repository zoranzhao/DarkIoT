#ifndef CLIENT_H
#define CLIENT_H
#include "config.h"
#include "exec_ctrl.h"

device_ctxt* init_client(uint32_t cli_id);
void steal_and_process_thread(void *arg);
void generate_and_process_thread(void *arg);
void send_result_thread(void *arg);
void serve_stealing_thread(void *arg);
void register_client(device_ctxt* ctxt);
void cancel_client(device_ctxt* ctxt);
void* steal_client(void* srv_conn, void *arg);

#endif
