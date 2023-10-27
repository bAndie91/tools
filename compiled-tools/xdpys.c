
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <err.h>
#include <dirent.h>
#include <sys/types.h>

#define LISTSIZE(x) (sizeof(x)/sizeof(*x)-1)
#define DPYNAME_MAXLEN 255


static jmp_buf xopendpy;
unsigned int timeout = 2;
unsigned int verbose = 0;
unsigned int found_a_valid_display = 0;


void alarm_handler(int sig)
{
	siglongjmp(xopendpy, 1);
}

unsigned int is_valid_xdisplay(char* dpyname)
{
	Display* dpy;

	if(verbose) fprintf(stderr, "Opening X display %s\n", dpyname);

	alarm(timeout);
	if(!sigsetjmp(xopendpy, 1))
	{
		dpy = XOpenDisplay(dpyname);
	}
	else
	{
		dpy = NULL;
	}
	alarm(0);
	if(dpy != NULL)
	{
		XCloseDisplay(dpy);
		found_a_valid_display = 1;
		printf("%s\n", dpyname == NULL ? getenv("DISPLAY") : dpyname);
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{
	Display* dpy;
	char dpyname[DPYNAME_MAXLEN];
	unsigned int num;
	int c;
	int search_abstract = 1;
	int search_sockets = 1;
	int search_inet_max = 30;
	FILE* fh;
	char socksbuf[92];
	DIR* dh;
	struct dirent* dent;
	unsigned int u;


	while((c = getopt(argc, argv, "hvASt:x:")) != -1)
	{
		switch(c)
		{
			case 't':
				timeout = atoi(optarg);
			break;
			case 'v':
				verbose = 1;
			break;
			case 'A':
				search_abstract = 0;
			break;
			case 'S':
				search_sockets = 0;
			break;
			case 'x':
				search_inet_max = atoi(optarg);
			break;
			case 'h':
				printf("Usage: xdpys [-h|-v|-A|-S] [-t <sec>] [-x <max>] [display [display [...]]]\n"
" -h       this help\n"
" -v       verbose output to stderr\n"
" -A       don't test abstract unix sockets\n"
" -S       don't test unix socket files\n"
" -t <n>   timeout for individual XDisplay open calls (%d)\n"
" -x <n>   search this many X.org display in INET namespace (%d)\n",
					timeout,
					search_inet_max);
				_exit(EXIT_SUCCESS);
			break;
			case '?':
				_exit(EXIT_FAILURE);
			break;
		}
	}


	signal(SIGALRM, alarm_handler);

	if(optind >= argc)
	{
		if(search_abstract)
		{
			fh = fopen("/proc/net/unix", "r");
			if(fh != NULL)
			{
				while(!feof(fh))
				{
					fgets(socksbuf, 91, fh);
					// e5d1fa40: 00000002 00000000 00010000 0001 01 187671 @/tmp/.X11-unix/X1003
					// e697ba80: 00000002 00000000 00010000 0001 01 20247 @/tmp/.X11-unix/X1000
					if(sscanf(socksbuf, "%*[^ ] %*[^ ] %*[^ ] %*[^ ] %*[^ ] 01 %*[^ ] @/tmp/.X11-unix/X%u", &u) == 1)
					{
						snprintf(dpyname, DPYNAME_MAXLEN, ":%u", u);
						is_valid_xdisplay(dpyname);
					}
					while(socksbuf[strlen(socksbuf)-1] != '\n')
					{
						fgets(socksbuf, 91, fh);
					}
				}
			}
			fclose(fh);
		}

		if(search_sockets)
		{
			dh = opendir("/tmp/.X11-unix");
			while((dent = readdir(dh)) != NULL)
			{
				if(dent->d_type == DT_SOCK && sscanf(dent->d_name, "X%u", &u) == 1)
				{
					snprintf(dpyname, DPYNAME_MAXLEN, ":%u", u);
					is_valid_xdisplay(dpyname);
				}
			}
			closedir(dh);
		}

		for(num = 0; num < search_inet_max; num++)
		{
			snprintf(dpyname, DPYNAME_MAXLEN, "localhost:%d", num);
			is_valid_xdisplay(dpyname);
		}
	}
	else
	{
		while(optind < argc)
		{
			is_valid_xdisplay(argv[optind]);
			optind++;
		}
	}
	
	return found_a_valid_display ? EXIT_SUCCESS : EXIT_FAILURE;
}
