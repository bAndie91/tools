
#define _POSIX_C_SOURCE 200809L

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <err.h>

/*
   This lib implements the "aborting" versions of
    - malloc
    - realloc
    - strdup
    - strndup
    - snprintf
   The program gets abort()ed if they can not allocate enough memory.
   Thus the caller don't need to pay attention to returning NULL pointers.
   In case of standard functions which does not allocate memory normally,
   like snprintf(3), it aborts if the given buffer size was not large enough.
   See abort(3).
 */

void* mallocab(size_t size)
{
	void* ptr;
	ptr = malloc(size);
	if(size != 0 && ptr == NULL)
	{
		warnx("Failed to allocate %zu bytes of memory.", size);
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
		warnx("Failed to reallocate %zu bytes of memory.", size);
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
		warnx("Failed to duplicate %zu bytes from %p.", size, ptr0);
		abort();
	}
	return ptr;
}

int snprintfab(char *dst, size_t size, const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int r = vsnprintf(dst, size, fmt, args);
	va_end(args);
	if (r < 0 || (size_t)r >= size) abort();
	return r;
}
