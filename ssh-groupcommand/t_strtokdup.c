
/*
 * This is a test program which calls strtokdup() directly.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "libmallocab.h"
#include "libstrtokdup.h"


int main(int argc, char** argv)
{
	char* t = strtokdup(argv[1], atoi(argv[2]));
	printf("strtokdup('%s', %d) -> '%s'\n", argv[1], atoi(argv[2]), t);
	free(t);
	return 0;
}
