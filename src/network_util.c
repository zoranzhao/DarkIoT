#include "network_util.h"

static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen);
static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen);

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
	printf("Control protocol is not supported");
	exit(EXIT_FAILURE);
   }
   if (sockfd < 0) {
	printf("ERROR opening socket");
	exit(EXIT_FAILURE);
   }
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
	printf("ERROR on binding");
	exit(EXIT_FAILURE);
   }
   if (proto == TCP) listen(sockfd, 10);//back_log numbers 
   return sockfd;
}


blob* recv_data(int sockfd, ctrl_proto proto){
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
      if (newsockfd < 0) printf("ERROR on accept");
      read_from_sock(newsockfd, TCP, (uint8_t*)&bytes_length, sizeof(bytes_length), NULL, NULL);
      buffer = (uint8_t*)malloc(bytes_length);
      read_from_sock(newsockfd, TCP, buffer, bytes_length, NULL, NULL);
      close(newsockfd);   
   }else if(proto == UDP){
      read_from_sock(sockfd, UDP, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) &cli_addr, &clilen);
      buffer = (uint8_t*)malloc(bytes_length);
      read_from_sock(sockfd, UDP, buffer, bytes_length, (struct sockaddr *) &cli_addr, &clilen);
   }else{ printf("Protocol is not supported\n"); return NULL;}
   close(sockfd);
   blob* tmp = new_blob_and_copy_data(0, bytes_length, buffer);
   free(buffer);
   return tmp;

}

void send_data(blob *temp, ctrl_proto proto, const char *dest_ip, int portno){
   void* data;
   uint32_t bytes_length;
   data = temp->data;
   bytes_length = temp->size;
/*lwIP implementation*/
/* ip_addr_t dstaddr; */

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
      printf("ERROR connecting");
   } else if (proto == UDP) {
#if IPV4_TASK
      sockfd = socket(AF_INET, SOCK_DGRAM, 0);
#elif IPV6_TASK//IPV4_TASK
      sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
#endif//IPV4_TASK
   }
   else {printf("Control protocol is not supported"); return;}
   if (sockfd < 0) printf("ERROR opening socket");

   write_to_sock(sockfd, proto, (uint8_t*)&bytes_length, sizeof(bytes_length), (struct sockaddr *) &serv_addr, sizeof(serv_addr));
   write_to_sock(sockfd, proto, (uint8_t*)data, bytes_length, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

   close(sockfd);
}

#define UDP_TRANS_SIZE 512
static inline void read_from_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, struct sockaddr *from, socklen_t *fromlen){
   uint32_t bytes_read = 0;
   int32_t n;
   while (bytes_read < bytes_length){
      if(proto == TCP){
         n = recv(sock, buffer + bytes_read, bytes_length - bytes_read, 0);
         if( n < 0 ) printf("ERROR reading socket");
      }else if(proto == UDP){
         if((bytes_length - bytes_read) < UDP_TRANS_SIZE) { n = bytes_length - bytes_read; }
         else { n = UDP_TRANS_SIZE; }
         if( recvfrom(sock, buffer + bytes_read, n, 0, from, fromlen) < 0) printf("ERROR reading socket");
      }else{printf("Protocol is not supported\n");}
      bytes_read += n;
   }
}

static inline void write_to_sock(int sock, ctrl_proto proto, uint8_t* buffer, uint32_t bytes_length, const struct sockaddr *to, socklen_t tolen){
   uint32_t bytes_written = 0;
   int32_t n;
   while (bytes_written < bytes_length) {
      if(proto == TCP){
         n = send(sock, buffer + bytes_written, bytes_length - bytes_written, 0);
         if( n < 0 ) printf("ERROR writing socket");
      }else if(proto == UDP){
         if((bytes_length - bytes_written) < UDP_TRANS_SIZE) { n = bytes_length - bytes_written; }
         else { n = UDP_TRANS_SIZE; }
         if(sendto(sock, buffer + bytes_written, n, 0, to, tolen)< 0) 
	   printf("ERROR writing socket\n");
      }else{printf("Protocol is not supported\n"); return;}
      bytes_written += n;
   }
}

