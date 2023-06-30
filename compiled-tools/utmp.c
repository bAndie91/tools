
#include <utmp.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <paths.h>

int main(int argc, char **argv)
{
	struct utmp *utmpptr;
	
	#ifdef DEBUG
	printf("EMPTY=%u RUN_LVL=%u BOOT_TIME=%u NEW_TIME=%u OLD_TIME=%u INIT_PROCESS=%u LOGIN_PROCESS=%u USER_PROCESS=%u DEAD_PROCESS=%u ACCOUNTING=%u\n", EMPTY, RUN_LVL, BOOT_TIME, NEW_TIME, OLD_TIME, INIT_PROCESS, LOGIN_PROCESS, USER_PROCESS, DEAD_PROCESS, ACCOUNTING);
	#endif
	
	if(argc > 1)
	{
		if(strcmp(argv[1], "--help")==0)
		{
			printf("Usage: utmp [<FILE>]\nDisplay bare UTMP database line-by-line.\nDefault FILE is "_PATH_UTMP".\n");
			return(0);
		}
		
		if(utmpname(argv[1]) != 0)
		{
			err(errno || -1, "%s: could not set utmp name", argv[1]);
		}
	}
	
	while((utmpptr = getutent()))
	{
		#ifdef DEBUG
		printf("utmp type=%u pid=%u line=%s id=%s user=%s host=%s\n",utmpptr->ut_type,utmpptr->ut_pid,utmpptr->ut_line,utmpptr->ut_id,utmpptr->ut_user,utmpptr->ut_host);
		#endif
		if(utmpptr->ut_type == USER_PROCESS)
		{
			printf("%u\t%s\t%s\t%s\n", utmpptr->ut_pid, utmpptr->ut_line, utmpptr->ut_user, utmpptr->ut_host);
		}
	}
	endutent();
	
	return(0);
}
