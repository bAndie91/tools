
#include <errno.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <libintl.h>
#include <locale.h>
#include <stdbool.h>

static const char* helptext = "Usage: remove [-v] <PATH> [<PATH> [...]]\n"
	"Remove files and directories alike.\n";

int main(int argc, char** argv)
{
	int fail;
	int idx = 0;
	bool verbose = false;
	
	setlocale(LC_ALL, "");
	
	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf(helptext);
		return 0;
	}
	
	if(argc >= 2 && (strcmp(argv[1], "-v")==0 || strcmp(argv[1], "--verbose")==0))
	{
		idx++;
		verbose = true;
	}
	
	if(argc < 2)
	{
		fprintf(stderr, helptext);
		return 1;
	}
	
	while(argv[++idx] != NULL)
	{
		fail = remove(argv[idx]);
		if(fail != 0) err(errno, "%s", argv[idx]);
		if(verbose) warnx(dcgettext("coreutils", "%s: removed", LC_MESSAGES), argv[idx]);
	}
}
