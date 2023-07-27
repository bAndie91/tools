
#ifndef _INCLUDE_LIBARRAY_H
#define _INCLUDE_LIBARRAY_H

struct array {
  char ** item;
  size_t size;  // size allocated for 'item'
  size_t length;  // how many items in 'item'
};

typedef struct array Array;

enum array_loop_control {
  ARRAY_LOOP_CONTINUE = 0,
  ARRAY_LOOP_STOP = 1,
};

typedef enum array_loop_control array_loop_control;

void array_init(Array**, size_t);
void array_setitem(Array**, size_t, char*);
void array_append(Array**, char*);
void array_insert(Array**, size_t index, char*);
#define array_prepend(array, item) array_insert(array, 0, item)
char* array_getitem(Array**, size_t);
char** array_getarray(Array**);
void array_delete(Array**, size_t, size_t);
char* array_pop(Array**, size_t);
void array_remove(Array**, const char*);
void array_empty(Array**);
void array_free(Array**);
void array_foreach(Array**, array_loop_control (size_t, char*, void*), void*);

#endif
