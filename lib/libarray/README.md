# libarray

This is a small C library (link your program against libarray.so) that provides primitive dynamic-array management functions.

- code namespace: `array_*`, `ARRAY_*`
- types
  - `array_loop_control`
  - `Array`

## Usage

The `array_*` functions operate on `Array**` pointers.
Memory for the array and its elements is managed by libarray.

`array_init()` allocates and initializes the `Array` structure.
It accepts an initial size (pass `0` if you do not know what starting size would be fitting);
`libarray` always grow and shrink the internal memory allocation automatically.

`array_free()` frees every stored item, releases the internal buffers and the
`Array` structure itself, and sets the caller's pointer to NULL.

Minimal usage:

```
#include <tool/libarray.h>
Array* my_array;
array_init(&my_array, 0);
array_free(&my_array);
```

`array_append()` makes an internal copy of the provided string;
so you retain ownership of the original buffer and may free or reuse it after the function call.
Appending `NULL` is supported and preserved as a valid array element.

The following example shows how to pass an oversized string.
The library duplicates input strings (via `strdupab`),
so items stored in the array use only as much memory as needed.

```
array_append(&my_array, "lorem ipsum");

char* item = malloc(100);
sprintf(item, "%s", "dolor sit amet");
array_append(&my_array, item);
free(item);

array_append(&my_array, NULL);
```

`array_getitem()` returns the item at the given index, or `NULL` if the index is out of bounds;
it also returns `NULL` when the element stored at that index is itself `NULL`.
The returned pointer is owned by libarray and must not be freed by the caller.

`array_pop()` removes and returns the item at the specified index.
Ownership of the returned pointer is transferred to the caller, so the caller must free it when no longer needed.
`array_shift()` is a convenience wrapper that pops the item at index 0.

If a function returns `NULL`, callers may check the array length with `array_length()` to distinguish between an out-of-bounds access and a stored `NULL` element.

`array_getarray()` returns a pointer to the internal `char **` buffer.
The caller may read elements through this pointer but must not free or reallocate the buffer.
The internal buffer must be considered invalid after any modifying operation (`array_append()`, `array_insert()`, `array_delete()`, etc.).

`array_slice()` creates and returns a new `Array *` containing copies of the
items from the requested range of the source array.
The caller owns the returned `Array` and its items and must call `array_free()` when finished.

`array_insert()` inserts a copy of the given item at the specified index and shifts subsequent elements upward.
If `index` is beyond the current length, the array is extended and the gap is filled with `NULL` elements.

`array_foreach()` invokes a user-provided callback for each element, starting at `start_index`.
The callback receives the current index, the item pointer (which may be `NULL`), and the user-supplied data pointer.
Returning `ARRAY_LOOP_STOP` from the callback stops iteration early; return
`ARRAY_LOOP_CONTINUE` to proceed.

```
array_index_t starting_index = 0;
void* cb_data = NULL;
array_loop_control callback(array_index_t current_index, char* current_item, void* cb_data) {
  // read/write current_item and/or cb_data
  return ARRAY_LOOP_CONTINUE;
}
array_foreach(&my_array, starting_index, callback, cb_data);
```

## Test coverage

A small unit-test runner is available at `lib/libarray/tests/t_libarray_unit.c`.
The single-file test exercises the public API (init, append, set/insert,
delete/pop/shift/remove, condense/empty, foreach) and a couple of internal
helpers (`_array_grow` / `_array_contract`). Edge cases covered, including:

- out-of-bounds indices, large insert/delete gaps, and stored `NULL` items, repeated appends, and condensing all‑NULL contents
- behavior while the array grows and contracts under load
- ownership rules (e.g. `array_pop()` returns a caller-owned pointer, `array_setitem()` replaces and frees the old item)
- foreach start offsets and early stop control

Build and run the tests with:

```sh
make unit-tests
```

This compiles the implementation into the test binary and executes the assertions.
After changing `libarray.c` you'll want to rerun the tests (or invoke `make` in the tests directory) to ensure no regressions.
The test binary exits non-zero on failure and prints the failed assertion's location.

The test runner is intentionally minimal;
it could be replaced with a richer framework (Check, cmocka) if you want fixtures, mocking or improved output.
The existing tests provide basic regression coverage and should catch most logic errors.
