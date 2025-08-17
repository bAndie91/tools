
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

#define DELIMITED_STR_LOOP_BEGIN(delimited_str_ptr, delimiter_ptr, delimiter_chr) \
	while(delimited_str_ptr != NULL && delimited_str_ptr[0] != '\0') { \
		delimiter_ptr = strchr(delimited_str_ptr, delimiter_chr);

#define DELIMITED_STR_LOOP_END(delimited_str_ptr, delimiter_ptr, delimiter_chr) \
		if(delimiter_ptr == NULL) break; \
		delimited_str_ptr = delimiter_ptr + 1; }


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
	
	int num_keep_envs = 0;
	char * keep_envs;
	char * comma;
	keep_envs = getenv("RUNAS_KEEP_ENVS");
	DELIMITED_STR_LOOP_BEGIN(keep_envs, comma, ',')
		num_keep_envs++;
	DELIMITED_STR_LOOP_END(keep_envs, comma, ',')
	
	int env_idx = 0;
	char ** envs = calloc(8 + num_keep_envs, sizeof(char*));
	if(envs == NULL) { abort(); }
	
	envs[env_idx++] = new_string("PATH=%s", getenv("PATH"));
	envs[env_idx++] = new_string("HOME=%s", pw->pw_dir);
	envs[env_idx++] = new_string("SHELL=%s", pw->pw_shell);
	envs[env_idx++] = new_string("USER=%s", USERNAME);
	envs[env_idx++] = new_string("LOGNAME=%s", USERNAME);
	if(getenv("TERM") != NULL)     { envs[env_idx++] = new_string("TERM=%s",    getenv("TERM")); }
	if(getenv("TERMCAP") != NULL)  { envs[env_idx++] = new_string("TERMCAP=%s", getenv("TERMCAP")); }
	
	keep_envs = getenv("RUNAS_KEEP_ENVS");
	char * envname;
	DELIMITED_STR_LOOP_BEGIN(keep_envs, comma, ',')
		envname = new_string("%.*s", comma - keep_envs, keep_envs);
		if(getenv(envname) != NULL) envs[env_idx++] = new_string("%s=%s", envname, getenv(envname));
	DELIMITED_STR_LOOP_END(keep_envs, comma, ',')
	
	envs[env_idx] = NULL;
	
	errno = 0;
	execvpe(COMMAND, &COMMAND, envs);
	error(125+errno, errno, "execvp: %s", COMMAND);
}
