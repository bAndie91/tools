
#ifndef _INCLUDE_LIBARRAY_H
#define _INCLUDE_LIBARRAY_H

#include <stddef.h>
#include <stdlib.h>


typedef size_t array_index_t;
typedef size_t array_length_t;

typedef struct {
  char ** item;
  array_index_t size;  // size allocated for 'item'
  array_length_t length;  // how many items in 'item' (both NULL and not NULL)
} Array;

typedef enum {
  ARRAY_LOOP_CONTINUE = 0,
  ARRAY_LOOP_STOP = 1,
} array_loop_control;

void array_init(Array**, array_index_t);
char* array_getitem(Array**, array_index_t) /* returned pointer owned by libarray; caller must NOT free */;
char** array_getarray(Array**);
array_length_t array_length(Array**);
void array_setitem(Array**, array_index_t, char* /* duplicated; caller keeps ownership */);
void array_append(Array**, char* /* duplicated; caller keeps ownership */);
void array_insert(Array**, array_index_t index, char* /* duplicated; caller keeps ownership */);
#define array_prepend(array, item) array_insert(array, 0, item)
void array_delete(Array**, array_index_t, array_length_t);
char* array_pop(Array**, array_index_t) /* returned pointer's ownership transfered to the caller, thus caller is responsible to free it */;
#define array_shift(array) array_pop(array, 0)
void array_remove(Array**, const char*);
void array_empty(Array**);
void array_free(Array**);
void array_foreach(Array**, array_index_t, array_loop_control (array_index_t, char*, void*), void*);

#endif
