
#include <libmallocab.h>
#include "libarray.h"

void array_init(Array** array, size_t initial_size)
{
	(*array) = mallocab(sizeof(Array));
	(*array)->size = initial_size;
	(*array)->item = mallocab((*array)->size * sizeof((*array)->item));
	(*array)->length = 0;
}

void array_append(Array** array, char * item)
{
	if((*array)->size < (*array)->length + 1)
	{
		if((*array)->size <= 0) (*array)->size = 1;
		(*array)->size *= 2;
		(*array)->item = reallocab((*array)->item, (*array)->size * sizeof((*array)->item));
	}
	(*array)->item[(*array)->length] = item == NULL ? NULL : strdupab(item);
	(*array)->length ++;
}

char* array_getitem(Array** array, size_t index)
{
	if(index < (*array)->length) return (*array)->item[index];
	else return NULL;
}

char** array_getarray(Array** array)
{
	return (*array)->item;
}

/* remove number of 'gap' items starting from 'index' */
void array_delete(Array** array, size_t index, size_t gap)
{
	size_t it;
	for(it = index; it < (*array)->length - gap; it++)
	{
		if(it - index < gap) free((*array)->item[it]);
		(*array)->item[it] = (*array)->item[it + 1];
	}
	(*array)->length -= gap;
}

/* remove 1 item from 'index' and returning with it */
/* the caller must free it */
char* array_pop(Array** array, size_t index)
{
	size_t it;
	char * item;
	item = NULL;
	if(index < (*array)->length) item = (*array)->item[index];
	for(it = index; it < (*array)->length - 1; it++)
	{
		(*array)->item[it] = (*array)->item[it + 1];
	}
	(*array)->length -= 1;
	return item;
}

/* remove an item which matches to the given 'item' */
void array_remove(Array** array, const char * item)
{
	size_t it;
	for(it = 0; it < (*array)->length; it++)
	{
		if(strcmp((*array)->item[it], item)==0)
		{
			array_delete(array, it, 1);
			break;
		}
	}
}

void array_foreach(Array** array, array_loop_control (*callback) (size_t, char*, void*), void * cb_data)
{
	size_t it;
	array_loop_control c;
	for(it = 0; it < (*array)->length; it++)
	{
		c = callback(it, (*array)->item[it], cb_data);
		if(c == ARRAY_LOOP_STOP) break;
	}
}

void array_empty(Array** array)
{
	size_t it;
	for(it = 0; it < (*array)->length; it++)
	{
		free((*array)->item[it]);
	}
}

void array_free(Array** array)
{
	array_empty(array);
	free((*array)->item);
	free((*array));
}
