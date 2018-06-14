#include "data_blob.h"

blob* new_blob_and_copy_data(int32_t id, uint32_t size, uint8_t* data)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = (uint8_t*)malloc(sizeof(uint8_t)*size);
   memcpy(temp->data, data, size);
   temp->size = size;
   temp->id = id;
   temp->free_data = 1;
   return temp; 
}

blob* new_blob_and_move_data(int32_t id, uint32_t size, uint8_t* data)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = data;
   temp->size = size;
   temp->id = id;
   temp->free_data = 0;
   return temp; 
}

blob* new_blob_and_alloc_data(int32_t id, uint32_t size)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = (uint8_t*)malloc(sizeof(uint8_t)*size);
   temp->size = size;
   temp->id = id;
   temp->free_data = 1;
   return temp; 
}

blob* new_empty_blob(int32_t id)
{
   blob *temp = (blob*)malloc(sizeof(blob));
   temp->data = NULL;
   temp->size = 0;
   temp->id = id;
   temp->free_data = 0;
   return temp; 
}

void free_blob(blob* temp){
   if((temp->data != NULL)&&(temp->free_data==1)){
      free(temp->data);
   }
   free(temp);
}
