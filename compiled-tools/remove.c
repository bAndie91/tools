
#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

static const char* helptext = "Usage: remove <PATH> [<PATH> [...]]\n"
	"Remove files and directories alike.\n";

int main(int argc, char** argv)
{
	int fail;
	int idx = 0;
	
	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf(helptext);
		return 0;
	}
	
	if(argc < 2)
	{
		fprintf(stderr, helptext);
		return 1;
	}
	
	while(argv[++idx] != NULL)
	{
		fail = remove(argv[idx]);
		if(fail == 0) continue;
		err(errno, "%s", argv[idx]);
	}
}
