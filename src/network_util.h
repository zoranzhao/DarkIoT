#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#define IPV4_TASK 1
#define IPV6_TASK !(IPV4_TASK)

#include "data_blob.h"

typedef enum proto{
   TCP,
   UDP
} ctrl_proto;

typedef struct service_connection{
   int sockfd;
   ctrl_proto proto;
   #if IPV4_TASK
   struct sockaddr_in* serv_addr_ptr;
   #elif IPV6_TASK/*IPV4_TASK*/
   struct sockaddr_in6* serv_addr_ptr;
   #endif/*IPV4_TASK*/   
} service_conn;

/*Networking API on service client side*/
service_conn* connect_service(ctrl_proto proto, const char *dest_ip, int portno);
void close_service_connection(service_conn* conn);
void send_request(void* meta, uint32_t meta_size, service_conn* conn);

/*Networking API on service server side*/
int service_init(int portno, ctrl_proto proto);
void start_service_for_n_times(int sockfd, ctrl_proto proto, const char* handler_name[], uint32_t handler_num, void* (*handlers[])(void*), uint32_t times);
void start_service(int sockfd, ctrl_proto proto, const char* handler_name[], uint32_t handler_num, void* (*handlers[])(void*));
void close_service(int sockfd);

/*Data exchanging API on both sides*/
blob* recv_data(service_conn* conn);
void send_data(blob *temp, service_conn* conn);

#endif
