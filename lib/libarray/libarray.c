
#include <libmallocab.h>

#include "libarray.h"

#define array_p (*array)

void array_init(Array** array, array_index_t initial_size)
{
	array_p = mallocab(sizeof(Array));
	array_p->size = initial_size;
	array_p->item = mallocab(array_p->size * sizeof(array_p->item));
	array_p->length = 0;
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

void array_append(Array** array, char * item)
{
	if(array_p->size < array_p->length + 1)
	{
		if(array_p->size <= 0) array_p->size = 1;
		array_p->size *= 2;
		array_p->item = reallocab(array_p->item, array_p->size * sizeof(array_p->item));
	}
	_array_setitem(array, array_p->length, item);
	array_p->length ++;
}

void array_insert(Array** array, array_index_t index, char * item)
{
	if(index >= array_p->length)
	{
		// TODO fill the gap in between when index > length
		array_append(array, item);
		return;
	}
	
	array_index_t cidx;
	array_append(array, NULL);
	for(cidx = array_p->length - 1; cidx > index; cidx--)
	{
		array_p->item[cidx] = array_p->item[cidx-1];
	}
	_array_setitem(array, index, item);
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
}

void array_free(Array** array)
{
	array_empty(array);
	free(array_p->item);
	array_p->item = NULL;
	free(array_p);
	array_p = NULL;
}

