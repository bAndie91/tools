
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>

int main(int argc, char* argv[])
{
	char * slash = strrchr(argv[0], '/');
	if(slash != NULL)
	{
		argv[0] = slash+1;
	}
	if(strlen(argv[0]) <= 5 || strncmp(argv[0], "suid-", 5)!=0)
	{
		errx(2, "Command name must start with 'suid-'.");
	}
	argv[0] += 5;
	execvp(argv[0], argv);
	err(127, "Could not execute: %s", argv[0]);
}
