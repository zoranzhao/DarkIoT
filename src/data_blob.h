#ifndef DATA_BLOB_H
#define DATA_BLOB__H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct ts_blob {
   uint8_t *data;
   uint32_t size;
   int32_t id;
   int8_t free_data;
} blob;

blob* new_blob_and_copy_data(int32_t id, uint32_t size, uint8_t* data);
blob* new_blob_and_move_data(int32_t id, uint32_t size, uint8_t* data);
blob* new_blob_and_alloc_data(int32_t id, uint32_t size);
blob* new_empty_blob(int32_t id);
void free_blob(blob* temp);

#endif
