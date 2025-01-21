
#include <dlfcn.h>
#include <errno.h>
#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>


#ifdef DEBUG
#define DEBUG(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__)
#else
#define DEBUG(...) 0
#endif

typedef int bool_t;
#define TRUE 1
#define FALSE 0

int main(int argc, char** argv)
{
	if(argc < 2) errx(2, "usage: getready <COMMAND> [<ARGS>]");
	raise(SIGSTOP);
	execvp(argv[1], &(argv[1]));
	err(127, "exec: %s", argv[1]);
}
