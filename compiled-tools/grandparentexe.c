
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <err.h>

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

int main()
{
	char *path;
	FILE *fh;
	char lbuf[32];
	pid_t gppid = 0;
	char linktarget[256];
	size_t len;
	
	if(asprintf(&path, "/proc/%d/status", getppid()) == -1) return(EXIT_FAILURE);
	fh = fopen(path, "r");
	if(!fh) { perror("fopen"); return(EXIT_FAILURE); }
	while(fgets(lbuf, ARRAYSIZE(lbuf), fh) != NULL)
	{
		if(sscanf(lbuf, "PPid: %d", &gppid) == 1)
		{
			break;
		}
	}
	fclose(fh);
	
	if(gppid == 0) { errx(EXIT_FAILURE, "Grandparent Pid not found"); }
	
	if(asprintf(&path, "/proc/%d/exe", gppid) == -1) return(EXIT_FAILURE);
	len = readlink(path, linktarget, ARRAYSIZE(linktarget));
	if(len == -1) { perror("readlink"); return(EXIT_FAILURE); }
	printf("%.*s\n", len, linktarget);
	
	return 0;
}
