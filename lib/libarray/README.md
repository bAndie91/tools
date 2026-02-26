# libarray

This is a C library (link your programm to libarray.so) providing primitive array-management functions.

- code namespace: `array_*`, `ARRAY_*`
- types
  - `array_loop_control`
  - `Array`

## Usage

`array_*` functions work with `Array**` pointers. Memory is managed by libarray.
`array_init()` allocates memory and initializes the pointer. `array_init` takes
initial size as argument; leave it 0 if unsure what initial size fits you best,
libarray will grow and shrink the memory area if and when needed.
`array_free()` frees up each item and the array itself too and nulls it out.

Minimal usage:

```
#include <tool/libarray.h>
Array* my_array;
array_init(&my_array, 0);
array_free(&my_array);
```

`array_append()` copies given string for itself, so you may free the passed string after the call, or even pass a constant to it.
You may append NULL too, it's preserved.

The following example shows how to pass an oversized string.
`libarray` duplicates the input item with `strdup` (well `strdupab`), so the string item stored in the array won't allocate more memory than needed.

```
array_append(&my_array, "lorem ipsum");

char* item;
item = malloc(100);
sprintf(item, "%s", "dolor sit amet");
array_append(&my_array, item);
free(item);

array_append(&my_array, NULL);
```

`array_getitem()` returns the item at the given index, or NULL if the index is out of bounds.
Caller must not free the resulting pointer.
`array_pop(&array, index)` removes and returns the item at the `index`th position
(which the caller should free, because it is removed from the array and no longer accounted by libarray),
and shift the rest of the items down by one.
The same stands for `array_shift()` too.
`array_shift()` shifts the items down and returns the 0th item.

Caller may want to check `array_length()` to decided that a NULL,
returned by one of the `array_pop()`/`array_shift()`/`array_getitem()` functions,
is NULL due to addressed the array out-of-bounds or just the item is itself NULL.

Caller can access the inner `char**` which holds the array of pointers to the elements
by `array_getarray()`, but should not free it (as it is not copied out from libarray) and
should not reuse it after the array is modified (by `array_append()`/`array_prepend()`/`array_insert()`/`array_delete()`/`array_remove()`/`array_pop()`/`array_shift()`/`array_condense()`...).

`array_insert()` inserts the given item at the given index, and shifts the rest of the items upward.
If the index is out of the upper bound, the item is still inserted to the `index`th position,
and all items between the old top-most item and the inserted item are set to NULL.

`array_foreach()` calls the user-provided callback function `callback` for each item in the array
with 3 arguments: index, item, and the user-provided callback data.
`callback()` may return `ARRAY_LOOP_STOP` to stop the loop.
Return `ARRAY_LOOP_CONTINUE` from `callback()` to continue going on.

```
array_index_t starting_index = 0;
void* cb_data = NULL;
array_loop_control callback(array_index_t current_index, char* current_item, void* cb_data) {
  // do something with the array or anything else...
  return ARRAY_LOOP_CONTINUE;
}
array_foreach(&my_array, starting_index, callback, cb_data);
```

## Test coverage

TODO
