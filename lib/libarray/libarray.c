
#include <libmallocab.h>
#include <string.h>

#include "libarray.h"

#define array_p (*array)

void array_init(Array** array, array_index_t initial_size)
{
	array_p = mallocab(sizeof(Array));
	array_p->size = initial_size;
	array_p->item = mallocab(array_p->size * sizeof(array_p->item));
	array_p->length = 0;
}

char* array_getitem(Array** array, array_index_t index)
{
	if(index < array_p->length) return array_p->item[index];
	else return NULL;
}

char** array_getarray(Array** array)
{
	return array_p->item;
}

array_length_t array_length(Array** array)
{
	return array_p->length;
}

static void _array_setitem(Array** array, array_index_t index, char* item)
{
	array_p->item[index] = item == NULL ? NULL : strdupab(item);
}

void array_setitem(Array** array, array_index_t index, char* item)
{
	if(index >= array_p->length)
	{
		array_insert(array, index, item);
		return;
	}
	if(array_p->item[index] != NULL) free(array_p->item[index]);
	_array_setitem(array, index, item);
}

/// @brief Grow the array's size to at least @p new_min_size, but maybe larger. Keep length unchanged.
void _array_grow(Array** array, array_length_t new_min_size)
{
	if(array_p->size < new_min_size)
	{
		if(array_p->size <= 0) array_p->size = 1;
		while(array_p->size < new_min_size) array_p->size *= 2;
		array_p->item = reallocab(array_p->item, array_p->size * sizeof(array_p->item));
	}
}

/// @brief Shrink the array's preallocated size to half of its size as many times as its occupated length allows.
void _array_contract(Array** array)
{
	while(array_p->size / 2 > array_p->length)
	{
		array_p->size /= 2;
		array_p->item = reallocab(array_p->item, array_p->size * sizeof(array_p->item));
	}
}

array_length_t array_condense(Array** array)
{
	array_index_t cidx;
	array_index_t move_to_idx;
	move_to_idx = 0;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		if(array_p->item[cidx] != NULL)
		{
			if(move_to_idx < cidx)
			{
				array_p->item[move_to_idx] = array_p->item[cidx];
			}
			move_to_idx++;
		}
	}
	array_p->length = move_to_idx;
	_array_contract(array);
	return array_p->length;
}

array_index_t array_append(Array** array, char * item)
{
	_array_grow(array, array_p->length + 1);
	_array_setitem(array, array_p->length, item);
	array_p->length ++;
	return array_p->length - 1;
}

void array_insert(Array** array, array_index_t index, char * item)
{
	array_index_t cidx;

	// Fill up the top of the array with NULLs if index is greater than the current length
	_array_grow(array, index + 1);
	for(cidx = array_p->length; cidx < index; cidx++)
	{
		_array_setitem(array, cidx, NULL);
	}

	for(cidx = array_p->length; cidx > index && cidx > 0; cidx--)
	{
		array_p->item[cidx] = array_p->item[cidx-1];
	}
	_array_setitem(array, index, item);
	if(array_p->length <= index)
		array_p->length = index + 1;
	else
		array_p->length++;
}

void array_delete(Array** array, array_index_t index, array_length_t gap)
{
	array_index_t cidx;
	for(cidx = index; cidx < array_p->length - gap; cidx++)
	{
		if(cidx - index < gap) free(array_p->item[cidx]);
		array_p->item[cidx] = array_p->item[cidx + gap];
	}
	array_p->length -= gap;
	_array_contract(array);
}

char* array_pop(Array** array, array_index_t index)
{
	array_index_t cidx;
	char * item;
	item = NULL;
	
	if(index < array_p->length)
	{
		item = array_p->item[index];
		
		for(cidx = index; cidx < array_p->length - 1; cidx++)
		{
			array_p->item[cidx] = array_p->item[cidx + 1];
		}
		
		array_p->length -= 1;
		_array_contract(array);
	}
	return item;
}

array_index_t array_remove(Array** array, const char * item)
{
	array_index_t cidx;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		if(array_p->item[cidx] != NULL && strcmp(array_p->item[cidx], item)==0)
		{
			array_delete(array, cidx, 1);
			break;
		}
	}
	return cidx;
}

void array_foreach(Array** array, array_index_t cidx, array_loop_control (*callback) (array_index_t, char*, void*), void * cb_data)
{
	array_loop_control c;
	for(; cidx < array_p->length; cidx++)
	{
		c = callback(cidx, array_p->item[cidx], cb_data);
		if(c == ARRAY_LOOP_STOP) break;
	}
}

void array_empty(Array** array)
{
	array_index_t cidx;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		if(array_p->item[cidx] != NULL)
		{
			free(array_p->item[cidx]);
			array_p->item[cidx] = NULL;
		}
	}
	array_p->length = 0;
	_array_contract(array);
}

void array_free(Array** array)
{
	array_empty(array);
	free(array_p->item);
	array_p->item = NULL;
	free(array_p);
	array_p = NULL;
}

