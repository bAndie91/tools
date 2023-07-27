
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

/*
   This lib implements the "aborting" versions of
    - malloc
    - realloc
    - strdup
    - strndup
   The program gets abort()ed if they can not allocate enough memory.
   Thus the caller don't need to pay attention to returning NULL pointers.
   See abort(3).
 */

void* mallocab(size_t size)
{
	void* ptr;
	ptr = malloc(size);
	if(size != 0 && ptr == NULL)
	{
		warnx("Failed to allocate %d bytes of memory.", size);
		abort();
	}
	return ptr;
}

void* reallocab(void* ptr0, size_t size)
{
	void* ptr;
	ptr = realloc(ptr0, size);
	if(size != 0 && ptr == NULL)
	{
		warnx("Failed to reallocate %d bytes of memory.", size);
		abort();
	}
	return ptr;
}

char* strdupab(const char* ptr0)
{
	char* ptr;
	ptr = strdup(ptr0);
	if(ptr == NULL)
	{
		warnx("Failed to duplicate %p.", ptr0);
		abort();
	}
	return ptr;
}

char* strndupab(const char* ptr0, size_t size)
{
	char* ptr;
	ptr = strndup(ptr0, size);
	if(ptr == NULL)
	{
		warnx("Failed to duplicate %d bytes from %p.", size, ptr0);
		abort();
	}
	return ptr;
}
