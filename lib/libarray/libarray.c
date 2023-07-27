
#include <libmallocab.h>
#include "libarray.h"

#define array_p (*array)

void array_init(Array** array, size_t initial_size)
{
	array_p = mallocab(sizeof(Array));
	array_p->size = initial_size;
	array_p->item = mallocab(array_p->size * sizeof(array_p->item));
	array_p->length = 0;
}

static void _array_setitem(Array** array, size_t index, char* item)
{
	array_p->item[index] = item == NULL ? NULL : strdupab(item);
}

void array_setitem(Array** array, size_t index, char* item)
{
	if(index < array_p->length) _array_setitem(array, index, item);
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

void array_insert(Array** array, size_t index, char * item)
{
	if(index >= array_p->length)
	{
		array_append(array, item);
		return;
	}
	
	size_t cidx;
	array_append(array, NULL);
	for(cidx = array_p->length - 1; cidx > index; cidx--)
	{
		array_p->item[cidx] = array_p->item[cidx-1];
	}
	_array_setitem(array, index, item);
}

char* array_getitem(Array** array, size_t index)
{
	if(index < array_p->length) return array_p->item[index];
	else return NULL;
}

char** array_getarray(Array** array)
{
	return array_p->item;
}

/* remove number of 'gap' items starting from 'index' */
void array_delete(Array** array, size_t index, size_t gap)
{
	size_t cidx;
	for(cidx = index; cidx < array_p->length - gap; cidx++)
	{
		if(cidx - index < gap) free(array_p->item[cidx]);
		array_p->item[cidx] = array_p->item[cidx + 1];
	}
	array_p->length -= gap;
}

/* remove 1 item from 'index' and returning with it */
/* the caller must free it */
char* array_pop(Array** array, size_t index)
{
	size_t cidx;
	char * item;
	item = NULL;
	if(index < array_p->length) item = array_p->item[index];
	for(cidx = index; cidx < array_p->length - 1; cidx++)
	{
		array_p->item[cidx] = array_p->item[cidx + 1];
	}
	array_p->length -= 1;
	return item;
}

/* remove an item which matches to the given 'item' */
void array_remove(Array** array, const char * item)
{
	size_t cidx;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		if(strcmp(array_p->item[cidx], item)==0)
		{
			array_delete(array, cidx, 1);
			break;
		}
	}
}

void array_foreach(Array** array, array_loop_control (*callback) (size_t, char*, void*), void * cb_data)
{
	size_t cidx;
	array_loop_control c;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		c = callback(cidx, array_p->item[cidx], cb_data);
		if(c == ARRAY_LOOP_STOP) break;
	}
}

void array_empty(Array** array)
{
	size_t cidx;
	for(cidx = 0; cidx < array_p->length; cidx++)
	{
		free(array_p->item[cidx]);
	}
}

void array_free(Array** array)
{
	array_empty(array);
	free(array_p->item);
	free(array_p);
}
