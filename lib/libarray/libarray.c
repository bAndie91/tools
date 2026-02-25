
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
	if(index < array_p->length)
	{
		if(array_p->item[index] != NULL) free(array_p->item[index]);
		_array_setitem(array, index, item);
	}
}

void array_grow(Array** array, array_length_t new_min_size)
{
	if(array_p->size < new_min_size)
	{
		if(array_p->size <= 0) array_p->size = 1;
		while(array_p->size < new_min_size) array_p->size *= 2;
		array_p->item = reallocab(array_p->item, array_p->size * sizeof(array_p->item));
	}
}

void array_append(Array** array, char * item)
{
	array_grow(array, array_p->length + 1);
	_array_setitem(array, array_p->length, item);
	array_p->length ++;
}

/* insert (a copy of) item into the index-th position */
/* index can be any non-negative integer; if larger than the current length, the array is grown and filled with NULLs */
void array_insert(Array** array, array_index_t index, char * item)
{
	array_index_t cidx;

	// Fill up the top of the array with NULLs if index is greater than the current length
	array_grow(array, index + 1);
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

/* remove number of 'gap' items starting from 'index' */
void array_delete(Array** array, array_index_t index, array_length_t gap)
{
	array_index_t cidx;
	for(cidx = index; cidx < array_p->length - gap; cidx++)
	{
		if(cidx - index < gap) free(array_p->item[cidx]);
		array_p->item[cidx] = array_p->item[cidx + gap];
	}
	array_p->length -= gap;
	// TODO array_shrink
}

/* remove 1 item from 'index' and returning it */
/* the caller should free it */
char* array_pick(Array** array, array_index_t index)
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
		// TODO array_shrink
	}
	return item;
}

char* array_pop(Array** array)
{
	return array_pick(array, array_p->length == 0 ? 0 : array_p->length - 1);
}

/* remove an item which matches to the given 'item' */
void array_remove(Array** array, const char * item)
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
	// TODO array_shrink
}

void array_free(Array** array)
{
	array_empty(array);
	free(array_p->item);
	array_p->item = NULL;
	free(array_p);
	array_p = NULL;
}

