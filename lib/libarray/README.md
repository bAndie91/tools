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
libarray will grow the memory area if needed.
However it does not shrink the freed area when elements are deleted.
`array_free()` frees up each element and the array itself too and nulls it out.

```
#include <tool/libarray.h>
Array* my_array;
array_init(&my_array, 0);
array_free(&my_array);
```

`array_append()` copies given string for itself, so you may pass constant to or free it up after the call.
You may add NULL too, it's preserved.

```
array_append(&my_array, "lorem ipsum");

char* item;
item = malloc(100);
sprintf(item, "%s", "dolor sit amet");
array_append(&my_array, item);
free(item);

array_append(&my_array, NULL);
```

`array_getitem()` returns the element at the given index, or NULL if the index is out of bounds.
Caller must not free the resulting pointer. `array_pop()` and `array_shift()` return the last and
the first element respectively; which the caller should free, because it is removed from the array
and no longer accounted by libarray. The same stands for `array_pick()` too.

Caller may want to check `array_length()` to decided that a NULL returned by one of the pop/shift/pick
functions is because the array is eaten up or just the picked element is itself NULL.

Caller can access the inner `char**` which holds the array of pointers to the elements
by `array_getarray()`, but should not free it (as it is not copied out from libarray) and
should not reuse it after the array is modified (append/insert/delete/remove/pick/pop/shift/...).

`array_foreach()` calls the user-provided callback function `callback` for each element in the array
with 3 arguments: index, element, and the user-provided callback data.
`callback()` must return `ARRAY_LOOP_STOP` if the array we're iterating on is changed, so the loop
referencing to just invalidated data may exit.

```
array_index_t starting_index = 0;
void* cb_data = NULL
array_loop_control callback(array_index_t index, char* element, void* cb_data) {
  if(/* my_array changed ... */) { return ARRAY_LOOP_STOP; }
  return ARRAY_LOOP_CONTINUE;
}
array_foreach(&my_array, starting_index, callback, cb_data);
```

## Test coverage

TODO
