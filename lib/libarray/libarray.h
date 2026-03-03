
#ifndef _INCLUDE_LIBARRAY_H
#define _INCLUDE_LIBARRAY_H

#include <stddef.h>
#include <stdlib.h>


typedef size_t array_index_t;
typedef size_t array_length_t;

typedef struct {
  char ** item;
  array_index_t size;  // allocated capacity of the 'item' buffer
  array_length_t length;  // number of slots in use (count of elements, including NULLs)
} Array;

typedef enum {
  ARRAY_LOOP_CONTINUE = 0,
  ARRAY_LOOP_STOP = 1,
  ARRAY_LOOP_REWIND = 2,
  ARRAY_LOOP_REPEAT = 3,
} array_loop_control;

/// @brief Create and initialize an Array and set the caller's pointer to it.
/// @param init_size initial capacity to allocate (pass 0 to use the library's defaults). The array will grow automatically as needed.
/// @brief The caller should call `array_free()` to release the returned Array when no longer needed.
void array_init(Array**, array_length_t init_size);

/// @return Return the item stored at @p index, or NULL if @p index is out of range.
/// @return Also returns NULL if the item at @p index is itself NULL.
/// The returned pointer is owned by libarray and must not be freed by the caller.
char* array_getitem(Array**, array_index_t index);

/// @return Return a pointer to the internal `char **` buffer.
/// The caller may read elements but must not modify, reallocate, or free the returned buffer.
/// The buffer becomes invalid after any modification on the array.
char** array_getarray(Array**);

/// @return Return the number of items in the array; this counts both NULL and non-NULL elements.
array_length_t array_length(Array**);

/// @brief Store a copy of @p item at @p index. The caller retains ownership of @p item;
/// libarray makes an internal copy. If @p index is beyond the current length,
/// the array is extended and the new indices are filled with NULL.
void array_setitem(Array**, array_index_t index, char* item);

/// @brief Append a copy of @p item to the end of the array.
/// The caller retains ownership of @p item; libarray makes an internal copy.
/// @return Return the index of the new element.
array_index_t array_append(Array**, char* item);

/// @brief Insert a copy of @p item at @p index position, shifting later elements upward.
/// @p index is a non-negative integer.
/// If @p index is larger than the current length the array is extended and the
/// gap is filled with NULL.
/// The caller retains ownership of @p item; libarray makes an internal copy.
void array_insert(Array**, array_index_t index, char* item);

/// @brief Insert @p item into the beginning of the array. The caller keeps ownership of @p item (libarray make a copy of it).
#define array_prepend(array, item) array_insert(array, 0, item)

/// @brief Remove number of @p gap items starting at `index`
/// and shift subsequent elements down to close the gap.
void array_delete(Array**, array_index_t index, array_length_t gap);

/// @brief Remove and return the item at @p index, shifting the subsequent elements down by 1.
/// Ownership of the returned pointer is transferred to the caller, who is then responsible for freeing it.
/// @return Return the removed item, or NULL if @p index is out of bounds.
char* array_pop(Array**, array_index_t index);

/// @brief Remove the first item of the array and return it to the caller; the caller should free the returned item (if not NULL).
#define array_shift(array) array_pop(array, 0)

/// @brief Remove the first element that matches (by string equality) to @p item.
/// @return Return the index of the removed item, or a value >= `array_length()` if no matching element was found.
array_index_t array_remove(Array**, const char* item);

/// @brief Remove all NULL items from the array, shifting non-NULL items to the left as much as possible.
/// @return Return the new length of the array.
array_length_t array_condense(Array**);

/// @brief Remove all items from the array.
/// The array's buffer remains allocated (but allocation is shrank internally).
void array_empty(Array**);

/// @brief Free every stored item, release internal buffers and the `Array` structure itself.
/// `array_free()` sets the caller's pointer to NULL.
void array_free(Array**);

/// @brief Iterate over the array starting at @p start_index, calling @p callback for each element.
/// The callback receives the @p current_index current index, the (possibly NULL) @p item item pointer, and the user-supplied @p cb_data data pointer.
/// The callback may return ARRAY_LOOP_STOP to terminate the iteration early;
/// ARRAY_LOOP_CONTINUE to keep iterating;
/// ARRAY_LOOP_REWIND to reset the iteration back to index 0 (not @p start_index); or
/// ARRAY_LOOP_REPEAT to repeat the current index (e.g. after deleting the item at that index).
void array_foreach(Array**, array_index_t start_index, array_loop_control (*callback)(Array** array, array_index_t current_index, char* item, void* cb_data), void* cb_data);

/// @brief Create a new `Array` containing copies of the string items
/// from @p start_index for up to @p length elements of the @p source_array.
/// The caller owns the returned array and must call `array_free()` when finished.
/// If @p start_index is outside the source array the returned array will be empty;
/// if @p length extends past the end of the source array the slice stops at the end.
Array* array_slice(Array** source_array, array_index_t start_index, array_length_t length);

#endif
