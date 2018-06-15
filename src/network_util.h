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
#define IPV4_TASK 0
#define IPV6_TASK !(IPV4_TASK)

#include "data_blob.h"

typedef enum proto{
   TCP,
   UDP
} ctrl_proto;
int service_init(int portno, ctrl_proto proto);
blob* recv_data(int sockfd, ctrl_proto proto);
void send_data(blob *temp, ctrl_proto proto, const char *dest_ip, int portno);

#endif
