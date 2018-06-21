#include "exec_ctrl.h"

void exec_start_gateway(int portno, ctrl_proto proto){
    char request_type[20] = "start_gateway";
    service_conn* conn = connect_service(proto, GATEWAY, portno);
    send_request(request_type, 20, conn);
    close_service_connection(conn);
}

void* start_gateway(void* conn){
   char request_type[20] = "start_edge";
   printf("Call start_gateway, start edge devices ...\n");
   uint32_t cli_id;
   service_conn* new_conn;
   for(cli_id = 0; cli_id < CLI_NUM; cli_id ++){
      new_conn = connect_service(((service_conn*)conn)->proto, addr_list[cli_id], START_CTRL);
      send_request(request_type, 20, new_conn);
      close_service_connection(new_conn);
   }
   return NULL;
}

void* start_edge(void* conn){
   printf("Call start_edge, Begin to do the work ...\n");
   return NULL;
}


void exec_barrier(int portno, ctrl_proto proto)
{
   const char* request_types[]={"start_gateway", "start_edge"};
   void* (*handlers[])(void*) = {start_gateway, start_edge};
   int service = service_init(portno, proto);
   start_service_for_n_times(service, proto, request_types, 2, handlers, 1);
   close_service(service);
}


