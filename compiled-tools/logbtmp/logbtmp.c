
#include <utmp.h>
#include <inttypes.h>
#include <stdio.h>
#include <iso646.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

/* Please get shadow's source code here. */
/* There are routines I want to reuse. */
#include "shadow-4.1.5.1/lib/getdef.c"

#define FTMP_FILE_DEFAULT "/var/log/btmp"
#define MISSING "???"
#define PAM_SYSTEM_ERR 4

/* Convert string to pid_t or exit. */
pid_t strtopid(const char* str)
{
	intmax_t xmax;
	char *tmp;
	pid_t pid;
	
	errno = 0;
	xmax = strtoimax(str, &tmp, 10);
	if(errno != 0 || tmp == str || *tmp != '\0' || xmax != (pid_t)xmax)
	{
		errx(EINVAL, "bad pid \"%s\"", str);
		/* Note, EINVAL == PAM_AUTHTOK_LOCK_BUSY */
	}
	return (pid_t)xmax;
}

int main(int argc, char** argv)
{
	struct utmp ut_entry;
	struct utmp *ut = &ut_entry;
	char *ftmp_path = NULL;
	char *ut_line = NULL;
	char *ut_pid = NULL;
	char *ut_user = NULL;
	char *ut_host = NULL;
	char *eq;
	int i;
	struct timeval tv;
	bool log_unkfail;
	
	log_unkfail = getdef_bool("LOG_UNKFAIL_ENAB");
	
	for(i = 1; i < argc; i++)
	{
		eq = strchr(argv[i], '=');
		if(eq != NULL)
		{
			eq++;
			if(strncmp(argv[i], "line=", 5)==0) {
				ut_line = eq;
			}
			else if(strncmp(argv[i], "pid=", 4)==0) {
				ut_pid = eq;
			}
			else if(strncmp(argv[i], "user=", 5)==0) {
				ut_user = eq;
			}
			else if(strncmp(argv[i], "host=", 5)==0) {
				ut_host = eq;
			}
			else if(strncmp(argv[i], "ftmp=", 5)==0) {
				ftmp_path = eq;
			}
			else if(strncmp(argv[i], "log_unkfail=", 12)==0) {
				if(strncmp(eq, "enab", 4)==0)
					log_unkfail = true;
				else if(strncmp(eq, "disa", 4)==0)
					log_unkfail = false;
				else goto bad_param;
			}
			else {
				goto bad_param;
			}
			continue;
		}
		
		bad_param:
		errx(PAM_SYSTEM_ERR, "Usage: logbtmp [line=<line>] [pid=<pid>] [user=<user>] [host=<host>] [ftmp=<filepath>] [log_unkfail=<enable|disable>]\n"
		"Missing parameters are defaulted to PAM_TTY, parent pid, PAM_USER, PAM_RHOST, and "FTMP_FILE_DEFAULT" respectively.");
	}
	
	if(ftmp_path == NULL) {
		ftmp_path = getdef_str("FTMP_FILE");
		if(ftmp_path == NULL) {
			ftmp_path = FTMP_FILE_DEFAULT;
		}
	}
	
	memzero(&ut_entry, sizeof(ut_entry));
	
	ut_entry.ut_type = USER_PROCESS;
	ut_entry.ut_pid = ut_pid ? strtopid(ut_pid) : getppid();
	ut_entry.ut_session = getsid(ut_entry.ut_pid);
	
	if(!ut_line) ut_line = getenv("PAM_TTY");
	if(!ut_line) ut_line = MISSING;
	strncpy(ut->ut_line, ut_line, UT_LINESIZE);
	if(strncmp(ut->ut_line, "tty", 3)==0) {
		strncpy(ut->ut_id, &ut->ut_line[3], sizeof(ut->ut_id));
	}
	
	if(!ut_user) ut_user = getenv("PAM_USER");
	if(ut_user && !log_unkfail && getpwnam(ut_user) == NULL) {
		ut_user = "UNKNOWN";
	}
	if(!ut_user) ut_user = MISSING;
	strncpy(ut->ut_user, ut_user, UT_NAMESIZE);
	
	if(!ut_host) ut_host = getenv("PAM_RHOST");
	if(!ut_host) ut_host = MISSING;
	strncpy(ut->ut_host, ut_host, UT_HOSTSIZE);
	
	if (gettimeofday (&tv, NULL) == 0) {
#ifdef HAVE_STRUCT_UTMP_UT_TIME
		ut->ut_time = tv.tv_sec;
#endif				/* HAVE_STRUCT_UTMP_UT_TIME */
#ifdef HAVE_STRUCT_UTMP_UT_XTIME
		ut->ut_xtime = tv.tv_usec;
#endif				/* HAVE_STRUCT_UTMP_UT_XTIME */
#ifdef HAVE_STRUCT_UTMP_UT_TV
		ut->ut_tv.tv_sec  = tv.tv_sec;
		ut->ut_tv.tv_usec = tv.tv_usec;
#endif				/* HAVE_STRUCT_UTMP_UT_TV */
	}
	
	if(open(ftmp_path, O_APPEND | O_WRONLY, 0) < 0) {
		err(errno, ftmp_path);
	}
	updwtmp(ftmp_path, ut);
	return EXIT_SUCCESS;
}
