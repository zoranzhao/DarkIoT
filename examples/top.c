#include "darkiot.h"
#include <string.h>
/*
make ARGS="2 start" test
make ARGS="2 wst gateway" test
make ARGS="0 wst_s data_source" test
make ARGS="1 wst non_data_source" test
make ARGS="2 wst non_data_source" test
*/
void test_gateway_ctrl(){
   exec_barrier(START_CTRL, TCP);
}

void test_edge_ctrl(){
   exec_barrier(START_CTRL, TCP);
}

void test_gateway(){

   init_gateway();
   char str[40]="Abracadabra, blablaboom";
   sys_thread_t t3 = sys_thread_new("work_stealing_thread", work_stealing_thread, str, 0, 0);
   sys_thread_t t1 = sys_thread_new("collect_result_thread", collect_result_thread, str, 0, 0);
   sys_thread_t t2 = sys_thread_new("merge_result_thread", merge_result_thread, str, 0, 0);
   exec_barrier(START_CTRL, TCP);

   sys_thread_join(t1);
   sys_thread_join(t2);
   sys_thread_join(t3);


}

void test_stealer_client(){
   exec_barrier(START_CTRL, TCP);
   init_client();
   sys_thread_t t1 = sys_thread_new("steal_and_process_thread", steal_and_process_thread, NULL, 0, 0);
   sys_thread_t t2 = sys_thread_new("send_result_thread", send_result_thread, NULL, 0, 0);

   sys_thread_join(t1);
   sys_thread_join(t2);

}

void test_victim_client(){
   exec_barrier(START_CTRL, TCP);
   init_client();
   sys_thread_t t1 = sys_thread_new("generate_and_process_thread", generate_and_process_thread, NULL, 0, 0);
   sys_thread_t t2 = sys_thread_new("send_result_thread", send_result_thread, NULL, 0, 0);
   sys_thread_t t3 = sys_thread_new("serve_stealing_thread", serve_stealing_thread, NULL, 0, 0);

   sys_thread_join(t1);
   sys_thread_join(t2);
   sys_thread_join(t3);


}

void test_wst(int argc, char **argv)
{
   printf("total_cli_num %d\n", atoi(argv[1]));
   printf("this_cli_id %d\n", atoi(argv[1]));

   this_cli_id = atoi(argv[1]);
   total_cli_num = atoi(argv[1]);
      
   if(0 == strcmp(argv[2], "start")){  
      printf("start\n");
      exec_start_gateway(START_CTRL, TCP);
   }else if(0 == strcmp(argv[2], "wst")){
      printf("Work stealing\n");
      if(0 == strcmp(argv[3], "non_data_source")){
         printf("non_data_source\n");
         test_stealer_client();
      }else if(0 == strcmp(argv[3], "data_source")){
         printf("data_source\n");
         test_victim_client();
      }else if(0 == strcmp(argv[3], "gateway")){
         printf("gateway\n");
         test_gateway();
      }
   }else if(0 == strcmp(argv[2], "wst_s")){
      printf("Work stealing with scheduling\n");
      if(0 == strcmp(argv[3], "non_data_source")){
         printf("non_data_source\n");
         test_stealer_client();
      }else if(0 == strcmp(argv[3], "data_source")){
         printf("data_source\n");
         test_victim_client();
      }else if(0 == strcmp(argv[3], "gateway")){
         printf("gateway\n");
         test_gateway();
      }
   }
}

int main(int argc, char **argv){
   /*test_queue_remove(argc, argv);*/
   test_wst(argc, argv);
   return 0;
}

