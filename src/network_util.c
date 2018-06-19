#include "network_util.h"

static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen);
static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen);

#if IPV4_TASK
static inline service_conn* new_service_conn(int sockfd, ctrl_proto proto, const char *dest_ip, struct sockaddr_in* addr, int portno){
#elif IPV6_TASK/*IPV4_TASK*/
static inline service_conn* new_service_conn(int sockfd, ctrl_proto proto, const char *dest_ip, struct sockaddr_in6* addr, int portno){
#endif/*IPV4_TASK*/ 
   service_conn* conn = (service_conn*)malloc(sizeof(service_conn)); 
   conn -> sockfd = sockfd;
   conn -> proto = proto;
   if(addr!=NULL){
      conn -> serv_addr_ptr = addr;
   }else{
      #if IPV4_TASK
      conn -> serv_addr_ptr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
      conn -> serv_addr_ptr -> sin_family = AF_INET;
      conn -> serv_addr_ptr -> sin_port = htons(portno);
      inet_pton(AF_INET, dest_ip, &(conn -> serv_addr_ptr -> sin_addr));
      #elif IPV6_TASK/*IPV4_TASK*/
      conn -> serv_addr_ptr = (struct sockaddr_in6*)malloc(sizeof(struct sockaddr_in6));
      conn -> serv_addr_ptr -> sin6_family = AF_INET6;
      conn -> serv_addr_ptr -> sin6_port = htons(portno);
      inet_pton(AF_INET6, dest_ip, &(conn -> serv_addr_ptr -> sin6_addr));
      #endif/*IPV4_TASK*/ 
   }
   return conn; 
}

int service_init(int portno, ctrl_proto proto){
   int sockfd;
#if IPV4_TASK
   struct sockaddr_in serv_addr;
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
#elif IPV6_TASK/*IPV4_TASK*/
   struct sockaddr_in6 serv_addr;
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin6_family = AF_INET6;
   serv_addr.sin6_addr = in6addr_any;
   serv_addr.sin6_port = htons(portno);
#endif/*IPV4_TASK*/
#if IPV4_TASK
   if (proto == UDP) sockfd = socket(AF_INET, SOCK_DGRAM, 0);
   else if (proto == TCP) sockfd = socket(AF_INET, SOCK_STREAM, 0);
#elif IPV6_TASK/*IPV4_TASK*/
   if (proto == UDP) sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
   else if (proto == TCP) sockfd = socket(AF_INET6, SOCK_STREAM, 0);
#endif/*IPV4_TASK*/
   else {
	printf("Control protocol is not supported\n");
	exit(EXIT_FAILURE);
   }
   if (sockfd < 0) {
	printf("ERROR opening socket\n");
	exit(EXIT_FAILURE);
   }
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	printf("ERROR on binding\n");
	exit(EXIT_FAILURE);
   }
   if (proto == TCP) listen(sockfd, 10);//back_log numbers 
   return sockfd;
}

service_conn* connect_service(ctrl_proto proto, const char *dest_ip, int portno){
   int sockfd;
#if IPV4_TASK
   struct sockaddr_in serv_addr;
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(portno);
   inet_pton(AF_INET, dest_ip, &serv_addr.sin_addr);
/*lwIP implementation*/
/*
   ip4addr_aton(dest_ip, ip_2_ip4(&dstaddr));
   inet_addr_from_ip4addr(&serv_addr.sin_addr, ip_2_ip4(&dstaddr));
*/
#elif IPV6_TASK//IPV4_TASK
   struct sockaddr_in6 serv_addr;
   memset(&serv_addr, 0, sizeof(serv_addr));
   serv_addr.sin6_family = AF_INET6;
   serv_addr.sin6_port = htons(portno);
   inet_pton(AF_INET6, dest_ip, &serv_addr.sin6_addr);
/*lwIP implementation*/
/*
   ip6addr_aton(dest_ip, ip_2_ip6(&dstaddr));
   inet6_addr_from_ip6addr(&serv_addr.sin6_addr, ip_2_ip6(&dstaddr));
*/
#endif//IPV4_TASK
   if(proto == TCP) {
#if IPV4_TASK
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
#elif IPV6_TASK//IPV4_TASK
      sockfd = socket(AF_INET6, SOCK_STREAM, 0);
#endif//IPV4_TASK
      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
      printf("ERROR connecting\n");
   } else if (proto == UDP) {
#if IPV4_TASK
      sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#elif IPV6_TASK//IPV4_TASK
      sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
#endif//IPV4_TASK
   }
   else {printf("Control protocol is not supported\n"); return NULL;}
   if (sockfd < 0) printf("ERROR opening socket\n");

   service_conn* conn = new_service_conn(sockfd, proto, dest_ip, NULL, portno);

   return conn;
}

void close_service_connection(service_conn* conn){
   close(conn -> sockfd);
   free(conn -> serv_addr_ptr);
   free(conn);
}

void send_data(blob *temp, service_conn* conn){
   void* data;
   uint32_t bytes_length;
   data = temp->data;
   bytes_length = temp->size;
   write_to_sock(conn->sockfd, conn->proto, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
   write_to_sock(conn->sockfd, conn->proto, (uint8_t*)data, bytes_length, (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
}

blob* recv_data(service_conn* conn){
   uint8_t* buffer;
   uint32_t bytes_length;
   socklen_t addr_len;
#if IPV4_TASK
   addr_len = sizeof(struct sockaddr_in);
#elif IPV6_TASK//IPV4_TASK
   addr_len = sizeof(struct sockaddr_in6);
#endif//IPV4_TASK
   read_from_sock(conn->sockfd, conn->proto, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) (conn->serv_addr_ptr), &addr_len);
   buffer = (uint8_t*)malloc(bytes_length);
   read_from_sock(conn->sockfd, conn->proto, buffer, bytes_length, (struct sockaddr *) (conn->serv_addr_ptr), &addr_len);
   blob* tmp = new_blob_and_copy_data(0, bytes_length, buffer);
   free(buffer);
   return tmp;
}

void send_request(void* meta, uint32_t meta_size, service_conn* conn){
   blob* temp = new_empty_blob(0);
   send_data_with_meta(meta, meta_size, temp, conn);
   free_blob(temp);  
}

void send_data_with_meta(void* meta, uint32_t meta_size, blob *temp, service_conn* conn){
   void* data;
   uint32_t bytes_length;
   void* meta_data;
   uint32_t meta_data_bytes_length;
   data = temp->data;
   meta_data = meta;
   bytes_length = temp->size;
   meta_data_bytes_length = meta_size;

   write_to_sock(conn->sockfd, conn->proto, (uint8_t*)&meta_data_bytes_length, sizeof(meta_data_bytes_length), (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
   write_to_sock(conn->sockfd, conn->proto, (uint8_t*)meta_data, meta_data_bytes_length, (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
   write_to_sock(conn->sockfd, conn->proto, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
   if(bytes_length > 0)
     write_to_sock(conn->sockfd, conn->proto, (uint8_t*)data, bytes_length, (struct sockaddr *) (conn->serv_addr_ptr), sizeof(struct sockaddr));
}


static inline uint32_t look_up_handler_table(char* name, const char* handler_name[], uint32_t handler_num){
   uint32_t handler_id = 0;
   for(handler_id = 0; handler_id < handler_num; handler_id++){
      if(strcmp(name, handler_name[handler_id]) == 0) break;
   }
   return handler_id;
}

blob* recv_request(int sockfd, ctrl_proto proto){
   socklen_t clilen;

#if IPV4_TASK
   struct sockaddr_in cli_addr;
#elif IPV6_TASK//IPV4_TASK
   struct sockaddr_in6 cli_addr;
#endif//IPV4_TASK

   int newsockfd;
   clilen = sizeof(cli_addr);
   uint32_t bytes_length;
   uint8_t* buffer;

   if(proto == TCP){
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0) {printf("ERROR on accept\n");return NULL;}
      read_from_sock(newsockfd, TCP, (uint8_t*)&bytes_length, sizeof(bytes_length), NULL, NULL);
      buffer = (uint8_t*)malloc(bytes_length);
      read_from_sock(newsockfd, TCP, buffer, bytes_length, NULL, NULL);
      close(newsockfd);   
   }else if(proto == UDP){
      read_from_sock(sockfd, UDP, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) &cli_addr, &clilen);
      buffer = (uint8_t*)malloc(bytes_length);
      read_from_sock(sockfd, UDP, buffer, bytes_length, (struct sockaddr *) &cli_addr, &clilen);
   }else{ printf("Protocol is not supported\n"); return NULL;}
   blob* tmp = new_blob_and_copy_data(0, bytes_length, buffer);
   free(buffer);
   /*This function is supposed to be called multiple times.*/
   /*close(sockfd);*/
   return tmp;
}

void start_service(int sockfd, ctrl_proto proto, const char* handler_name[], uint32_t handler_num, void* (*handlers[])(void*)){
   socklen_t clilen;

#if IPV4_TASK
   struct sockaddr_in cli_addr;
#elif IPV6_TASK//IPV4_TASK
   struct sockaddr_in6 cli_addr;
#endif//IPV4_TASK

   int newsockfd;
   clilen = sizeof(cli_addr);
   uint32_t bytes_length;
   uint8_t* buffer = NULL;
   uint32_t meta_data_bytes_length;
   uint8_t* meta_data;
   blob* tmp;

   while(1){
      uint32_t handler_id = 0;
      /*Accept incoming connection*/
      if(proto == TCP){
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      }else if(proto == UDP){
         newsockfd = sockfd;
      }else{ 
         printf("Protocol is not supported\n"); 
         return;
      }
      if (newsockfd < 0) {printf("ERROR on accept\n");return;}
      /*Accept incoming connection*/


      read_from_sock(newsockfd, proto, (uint8_t*)&meta_data_bytes_length, sizeof(meta_data_bytes_length), (struct sockaddr *) &cli_addr, &clilen);
      meta_data = (uint8_t*)malloc(meta_data_bytes_length);
      read_from_sock(newsockfd, proto, meta_data, meta_data_bytes_length, (struct sockaddr *) &cli_addr, &clilen);
      read_from_sock(newsockfd, proto, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) &cli_addr, &clilen);
      if(bytes_length > 0){
         buffer = (uint8_t*)malloc(bytes_length);
         read_from_sock(newsockfd, proto, buffer, bytes_length, (struct sockaddr *) &cli_addr, &clilen);
      }
      if(bytes_length > 0){
         tmp = new_blob_and_copy_data(0, bytes_length, buffer);
         free(buffer);  
      }else{
         tmp = new_empty_blob(0);
      }
      handler_id =  look_up_handler_table((char*)meta_data, handler_name, handler_num);      
      free(meta_data);
      if(handler_id == handler_num){printf("Operation is not supported!\n"); return;}
      (handlers[handler_id])(tmp);
      free_blob(tmp);  


      /*Close connection*/
      if(proto == TCP){
         close(newsockfd);     
      }
      /*Close connection*/
   }
   close(sockfd);
}

#define UDP_TRANS_SIZE 512
static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen){
   uint32_t bytes_read = 0;
   int32_t n = 0;
   while (bytes_read < bytes_length){
      if(proto == TCP){
         n = recv(sock, buffer + bytes_read, bytes_length - bytes_read, 0);
         if( n < 0 ) printf("ERROR reading socket\n");
      }else if(proto == UDP){
         if((bytes_length - bytes_read) < UDP_TRANS_SIZE) { n = bytes_length - bytes_read; }
         else { n = UDP_TRANS_SIZE; }
         if( recvfrom(sock, buffer + bytes_read, n, 0, from, fromlen) < 0) printf("ERROR reading socket\n");
      }else{printf("Protocol is not supported\n");}
      bytes_read += n;
   }
}

static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen){
   uint32_t bytes_written = 0;
   int32_t n = 0;
   while (bytes_written < bytes_length) {
      if(proto == TCP){
         n = send(sock, buffer + bytes_written, bytes_length - bytes_written, 0);
         if( n < 0 ) printf("ERROR writing socket\n");
      }else if(proto == UDP){
         if((bytes_length - bytes_written) < UDP_TRANS_SIZE) { n = bytes_length - bytes_written; }
         else { n = UDP_TRANS_SIZE; }
         if(sendto(sock, buffer + bytes_written, n, 0, to, tolen)< 0) 
	   printf("ERROR writing socket\n");
      }else{printf("Protocol is not supported\n"); return;}
      bytes_written += n;
   }
}

