
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

/// @brief Create and initialize an array with the given @p init_size initial size.
/// @brief The caller should free the created array by calling array_free() when it's no longer needed.
void array_init(Array**, array_length_t init_size);

/// @brief Get the item at the given @p index.
/// @returns NULL if @p index is out of range.
/// @returns NULL also if the item at @p index is itself NULL.
/// The returned pointer is owned by libarray; caller must NOT free it.
char* array_getitem(Array**, array_index_t index);

/// @brief Get the pointer to the internal array of items. The caller can access items by indexing this array, but should not modify the items or the array itself (e.g. by reallocating it).
char** array_getarray(Array**);

/// @brief Get the number of items in the array (both NULL and not NULL).
array_length_t array_length(Array**);

/// @brief Set the item at the given @p index to a copy of @p item. The caller keeps ownership of @p item (libarray make a copy of it).
/// @brief If @p index is larger than the current length, the array is extended and filled with NULLs up to @p index, and then the item is set.
void array_setitem(Array**, array_index_t index, char* item);

/// @brief Append (a copy of) @p item to the end of the array. The caller keeps ownership of @p item (libarray make a copy of it).
/// @returns the index of the appended item.
array_index_t array_append(Array**, char* item);

/// @brief Insert (a copy of) @p item into the @p index position.
/// @brief @p index can be any non-negative integer. If larger than the current length, the array is extended and filled with NULLs up to @p index.
/// @brief The caller keeps ownership of @p item (libarray make a copy of it).
void array_insert(Array**, array_index_t index, char* item);

/// @brief Insert @p item into the beginning of the array. The caller keeps ownership of @p item (libarray make a copy of it).
#define array_prepend(array, item) array_insert(array, 0, item)

/// @brief remove number of @p gap items starting from @p index, shifting the rest of the items down to close the gap.
void array_delete(Array**, array_index_t index, array_length_t gap);

/// @brief remove 1 item from @p index, shifting the rest, and returning that item to the caller; the caller should free the returned item
char* array_pop(Array**, array_index_t index) /* returned pointer's ownership transfered to the caller, thus caller is responsible to free it */;

/// @brief Remove the first item of the array and return it to the caller; the caller should free the returned item (if not NULL).
#define array_shift(array) array_pop(array, 0)

/// @brief remove the first item which matches (string equals) to the given @p item.
/// @returns the index of the removed item, or an out-of-bounds index if no item matched.
/// The returned index points to the item that was the next after the removed @p item, or beyond the array if the removed @p item was the last one or no item was removed.
array_index_t array_remove(Array**, const char* item);

/// @brief remove all NULL items from the array, shifting the non-NULL items to the left as much as possible, and return the new length of the array
array_length_t array_condense(Array**);

/// @brief Empty out the array, freeing all the items. 
void array_empty(Array**);

/// @brief Empty out the array and free the array itself. The caller should set the pointer to NULL after calling this function to avoid dangling pointer.
void array_free(Array**);

/// @brief Loop through the array, starting at @p start_index, and call the given @p callback function for each item.
/// @brief The loop can be stopped by returning ARRAY_LOOP_STOP from the callback. Return ARRAY_LOOP_CONTINUE to continue the loop. The callback function receives the current index, the item at that index, and the user-provided callback data as arguments.
void array_foreach(Array**, array_index_t start_index, array_loop_control (*callback)(array_index_t current_index, char* item, void* cb_data), void* cb_data);

#endif
