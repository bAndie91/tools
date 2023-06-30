
#include <err.h>
#include <errno.h>
#include <string.h>	//strdup
#include <stdio.h>	//printf
#include <fcntl.h>	//O_*
#define _GNU_SOURCE 1
#include <sys/syscall.h>


int syncfs(int fd)
{
	return syscall(SYS_syncfs, fd);
}

int main(int argc, char** argv)
{
	char* path;
	int narg = 1;
	int fd;
	
	if(argc > 1 && strcmp(argv[1], "--help")==0)
	{
		printf(
"Usage: syncfs [<PATH>]\n"
"Request sync to the disk for the filesystem which <PATH> is on.\n"
"<PATH> is the current directory by default.\n");
		return 0;
	}
	
	if(argc < 2)
	{
		path = strdup(".");
		goto have_path;
	}
	else
	{
		while(narg < argc)
		{
			path = argv[narg];
			
			have_path:
			fd = open(path, O_RDONLY);
			if(fd == -1)
			{
				err(errno, "%s: open", path);
			}
			else
			{
				int ok = syncfs(fd);
				if(ok != 0)
				{
					warn("%s", path);
				}
				close(fd);
			}
			narg++;
		}
	}
	return 0;
}
