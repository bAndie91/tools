
#ifndef _INCLUDE_LIBARRAY_H
#define _INCLUDE_LIBARRAY_H

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
void array_setitem(Array**, array_index_t, char*);
void array_append(Array**, char*);
void array_insert(Array**, array_index_t index, char*);
#define array_prepend(array, item) array_insert(array, 0, item)
char* array_getitem(Array**, array_index_t);
char** array_getarray(Array**);
void array_delete(Array**, array_index_t, array_length_t);
char* array_pick(Array**, array_index_t);
char* array_pop(Array**);
#define array_shift(array) array_pick(array, 0)
void array_remove(Array**, const char*);
void array_empty(Array**);
void array_free(Array**);
void array_foreach(Array**, array_index_t, array_loop_control (array_index_t, char*, void*), void*);
array_length_t array_length(Array**);

#endif
