
#include <strings.h>
#include <limits.h>
#include <stdio.h>

#define VERSION 0.5
#define MYNAME "relpath"

int main(int argc, char *argv[]) {

	unsigned int i = 1;
	unsigned int i2 = 0;
	unsigned int per = 0;
	unsigned int newline = 1;
	char commondir[_POSIX_PATH_MAX];
	char buff[_POSIX_PATH_MAX];
	char route[_POSIX_PATH_MAX];
	char realname1[_POSIX_PATH_MAX];
	char realname2[_POSIX_PATH_MAX];

	while(i < argc && argv[i][0] == '-') {
		if(strcmp(argv[i], "-n")==0) {
			newline = 0;
		}
		else if(strcmp(argv[i], "--help")==0) {
			printf("%s",
				"Usage: " MYNAME " [-n] <DIR-1> <DIR-2>\n"
				"Print a relative path from <DIR-2> to <DIR-1>. Useful for making relative symlinks.\n"
				"This command creates <DIR-2>/symlink.txt pointing to <DIR-1>/link_target.txt:\n"
				"  ln -s $(" MYNAME " <DIR-1> <DIR-2>)link_target.txt <DIR-2>/symlink.txt\n"
				"Options:\n"
				"  -n  do not print newline\n"
				"Exit codes:\n"
				"  0   success\n"
				"  1   a directory does not exist\n"
				"  2   parameter error\n");
		      exit(0);
		}
		i++;
	}
	
	if(i+1 < argc) {
		if(realpath(argv[i], realname1)==0) exit(1);
		i++;
		if(realpath(argv[i], realname2)==0) exit(1);
	}
	else {
		fprintf(stderr, MYNAME ": Parameter error\n");
		exit(2);
	}

	
	strcpy(buff, "");
	strcpy(commondir, "");
	strcpy(route, "");
#ifdef DEBUG
	fprintf(stderr, " %s\n %s\n", realname1, realname2);
#endif
	if(strcmp(realname1, realname2)==0) {
		// directories are identical!
		printf("./");
		if(newline) printf("\n");
		exit(0);
	}
	if(realname1[strlen(realname1)-1] != '/') strcat(realname1, "/");
	if(realname2[strlen(realname2)-1] != '/') strcat(realname2, "/");

	i = 0;
	while(strlen(realname1)>i && strlen(realname2)>i && realname1[i] == realname2[i]) {
		sprintf(buff, "%s%c", buff, realname1[i]);
		if(realname1[i]=='/') {
			strcat(commondir, buff);
			strcpy(buff, "");
			i2=i;
		}
		i++;
	}

	//route = "../"*(1+count("/",a2)).a1

	while(strlen(realname2)>=++i) {
		if(realname2[i]=='/') per++;
	}
	strcpy(buff, "");
	while(per>0) {
		strcat(buff, "../");
		per--;
	}
	strcpy(route, buff);

	while(strlen(realname1)>=++i2) {
		sprintf(route, "%s%c", route, realname1[i2]);
	}

#ifdef DEBUG
	fprintf(stderr, "%s\n", commondir);
#endif
	printf("%s", route);
	if(newline) printf("\n");

	exit(0);
}
