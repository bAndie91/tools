
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void invmal()
{
	fprintf(stderr, "Usage:  mksock [-s | -d] PATH\n");
	exit(1);
}

int main(int argc, char ** argv)
{
	struct sockaddr_un sun;
	int sockfd, r;
	int type = SOCK_DGRAM;
	char *sockname;

	if(argc == 1) invmal();
	sockname = argv[1];
	if(argv[1][0]=='-') {
		if(strcmp(argv[1],"-s")==0) type = SOCK_STREAM;
		else if(strcmp(argv[1],"-d")==0) type = SOCK_DGRAM;
		else invmal();
		if(argc == 2) invmal();
		sockname = argv[2];
	}

	memset(&sun, 0, sizeof(sun));
	sun.sun_family = AF_UNIX;
	snprintf(sun.sun_path, sizeof(sun.sun_path), "%s", sockname);

	sockfd = socket(PF_UNIX, type, 0);

	r = bind(sockfd, (struct sockaddr*)&sun, sizeof(sun));
	if(r != 0) {
		fprintf(stderr, "mksock: error making %s: %s\n", sun.sun_path, strerror(errno));
		exit(r);
	}

	return 0;
}
