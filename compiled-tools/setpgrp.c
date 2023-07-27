
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>


int main(int argc, char** argv)
{
	if(argc < 2)
	{
		errx(-1, "Usage: setpgrp <command> [arguments]");
	}
	
	/*
	The System V-style setpgrp(), which takes no arguments, is equivalent to setpgid(0, 0).
	*/
	
	int error = setpgid(0, 0);
	
	if(error != 0)
	{
		perror("setpgid");
		exit(error);
	}
	
	execvp(argv[1], &argv[1]);
	exit(127);
}
