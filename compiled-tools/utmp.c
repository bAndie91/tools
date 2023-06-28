
#include <utmp.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	struct utmp *utmpptr;

	if(argc == 2 && strcmp(argv[1], "--help")==0)
	{
		printf("Display UTMP database line-by-line.\n");
		return 0;
	}

	#ifdef DEBUG
	printf("EMPTY=%u RUN_LVL=%u BOOT_TIME=%u NEW_TIME=%u OLD_TIME=%u INIT_PROCESS=%u LOGIN_PROCESS=%u USER_PROCESS=%u DEAD_PROCESS=%u ACCOUNTING=%u\n", EMPTY, RUN_LVL, BOOT_TIME, NEW_TIME, OLD_TIME, INIT_PROCESS, LOGIN_PROCESS, USER_PROCESS, DEAD_PROCESS, ACCOUNTING);
	#endif
	
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
