
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <errno.h>
#include <error.h>

#define USERNAME (argv[1])
#define COMMAND (argv[2])

char * new_string(const char * fmt, ...)
{
	int ok;
	char * s;
	va_list ap;
	
	va_start(ap, fmt);
	ok = vasprintf(&s, fmt, ap);
	va_end(ap);
	
	if(ok == -1) { abort(); }
	return s;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		error(-1, 0, "Usage: %s <USER> <COMMAND> [<ARGS> ...]", argv[0]);
	}
	
	errno = 0;
	struct passwd *pw = getpwnam(USERNAME);
	
	if (errno)
	{
		error(errno, errno, "getpwnam: %s", USERNAME);
	}
	
	if (!pw)
	{
		error(0, 0, "getpwnam: %s: user not found", USERNAME);
		return 2;
	}
	
	errno = 0;
	if (initgroups(USERNAME, pw->pw_gid) != 0)
	{
		error(errno, errno, "initgroups: %s", USERNAME);
	}
	
	errno = 0;
	if (setgid(pw->pw_gid) != 0)
	{
		error(errno, errno, "setgid");
	}
	
	errno = 0;
	if (setuid(pw->pw_uid) != 0)
	{
		error(errno, errno, "setuid");
	}
	
	char ** envs = calloc(8, sizeof(char*));
	if(envs == NULL) { abort(); }
	
	envs[0] = new_string("PATH=%s", getenv("PATH"));
	envs[1] = new_string("HOME=%s", pw->pw_dir);
	envs[2] = new_string("SHELL=%s", pw->pw_shell);
	envs[3] = new_string("USER=%s", USERNAME);
	envs[4] = new_string("LOGNAME=%s", USERNAME);
	envs[5] = envs[6] = envs[7] = NULL;
	if(getenv("TERM") != NULL)     { envs[5] = new_string("TERM=%s",    getenv("TERM")); }
	if(getenv("TERMCAP") != NULL)  { envs[6] = new_string("TERMCAP=%s", getenv("TERMCAP")); }
	
	errno = 0;
	execvpe(COMMAND, &COMMAND, envs);
	error(125+errno, errno, "execvp: %s", COMMAND);
}
