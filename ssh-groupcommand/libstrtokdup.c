
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "libmallocab.h"

#define DEBUG_LIBSTRTOKDUP 0

/*
   Return a pointer to a copy of n-th word of a string.
   Return NULL if there are not enough words.
   Count starts at 1.
   Words are separated by whitespace
   Caller is responsible to free() the returning pointer.
 */

char* strtokdup(const char* s, unsigned int nth)
{
	char* r;
	unsigned int i, L;
	
	#if DEBUG_LIBSTRTOKDUP
	fprintf(stderr, "strtokdup('%s', %d)\n", s, nth);
	#endif
	if(s == NULL)
		return NULL;
	
	i = 0;
	/* Jump to nth token */
	while(nth > 0)
	{
		/* Strip leading whitespace */
		while(isspace(s[i]) && s[i] != '\0') i++;
		if(s[i] == '\0')
			return NULL;
		if(nth > 1)
		{
			/* Skip to next whitespace */
			while(!isspace(s[i]) && s[i] != '\0') i++;
			if(s[i] == '\0')
				return NULL;
		}
		nth--;
	}
	
	L = strcspn(s+i, " \f\n\r\t\v");
	r = mallocab(L+1);
	strncpy(r, s+i, L);
	r[L] = '\0';
	#if DEBUG_LIBSTRTOKDUP
	fprintf(stderr, " -> '%s'\n", r);
	#endif
	return r;
}
