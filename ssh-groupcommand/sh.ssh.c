
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <linux/limits.h>

#include "libmallocab.h"
#include "libstrtokdup.h"

#define FREE(p) {if(p!=NULL){free(p);p=NULL;}}while(0)
typedef int boolean;
#define FALSE 0
#define TRUE !FALSE

#define MSG_NOSHELL "Not allowed to run interactive shell."
#define MSG_NOCMDLN "Not allowed to run requested command line: %s"
#define MSG_NOWHATE "Not allowed to run whatever you want."


#define CONFFILE "/etc/ssh/AllowGroupCommands"
#define PARENTSFILE "/etc/ssh/AllowGroupStrictParents"
#define DEBUG 0



#if DEBUG
#define PRINTDEBUG(...) {warnx(__VA_ARGS__);}while(0)
#else
#define PRINTDEBUG(...) {}
#endif

boolean EQ(char* a, char* b)
{
	if(a==NULL || b==NULL || strcmp(a,b) != 0) return FALSE;
	return TRUE;
}

/* Trims the given string's last char if matches. */
boolean trimtrail(char* s, char t)
{
	if(s[strlen(s)-1] == t) {
		s[strlen(s)-1] = '\0';
		return TRUE;
	}
	return FALSE;
}

boolean gid_in_array(gid_t gid, gid_t* array, unsigned int size)
{
	unsigned int i;
	for(i = 0; i < size; i++)
	{
		if(array[i] == gid) return TRUE;
	}
	return FALSE;
}

boolean match(const char* str_a, const char* str_b)
{
	if(EQ(str_a, str_b)) return TRUE;
	if(EQ(str_a, "*")) return TRUE;
	return FALSE;
}

/* This macro calls parse_option() and continues next iteration in the loop if succeeds. */
#define PARSE_OPT_BOOL(x,y,z) if(parse_option_bool(x,y,z)){continue;}
boolean parse_option_bool(const char* configline, const char* option, boolean* variable)
{
	char* token1;
	boolean found = FALSE;
	token1 = strtokdup(configline, 1);
	if(EQ(token1, option))
	{
		char* token2;
		found = TRUE;
		token2 = strtokdup(configline, 2);
		if(EQ(token2, "off"))
		{
			PRINTDEBUG("unset option %s", option+1);
			*variable = FALSE;
		}
		else if(EQ(token2, "on"))
		{
			*variable = TRUE;
			PRINTDEBUG("set option %s", option+1);
		}
		FREE(token2);
	}
	FREE(token1);
	return found;
}

char* get_real_comm(const char* argv0, char** real_argv0)
{
	char* cp;
	char* real_comm;
	
	cp = strrchr(argv0, '/');
	if(cp == NULL)
	{
		/* Take our basename. */
		*real_argv0 = argv0;
	}
	else
	{
		/* Point to our basename. */
		*real_argv0 = cp+1;
	}
	/* Strip everything after the last dot, if there is one. */
	cp = *real_argv0;
	if(strrchr(cp, '.') != NULL)
	{
		*real_argv0 = strndupab(cp, strrchr(cp, '.') - cp);
	}
	/* If our name starts with a dash, let the command do not. */
	if((*real_argv0)[0] == '-')
	{
		real_comm = (*real_argv0)+1;
	}
	else
	{
		real_comm = *real_argv0;
	}
	return real_comm;
}


int main(int argc, char** argv, char** envp)
{
	char* real_comm;
	char** real_argv;
	struct passwd * pwent;
	struct group * grent;
	gid_t* group_ids;
	uid_t myuid;
	int n_groups, n;
	FILE* fh;
	#define LNBUFLEN 4096
	char lnbuf[LNBUFLEN];
	char* fc_string = NULL;
	char* sh_string = NULL;
	char* cmdline = NULL;
	boolean report_dotsshrc = FALSE;
	boolean strip_quotes = FALSE;
	pid_t parentpid;
	char pathbuf[PATH_MAX];
	char parentpath[PATH_MAX];
	ssize_t pathlen;
	char* cmd;
	
	/* Set how glibc responds when various kinds of programming errors are detected. */
	/* bit 0: print error message */
	/* bit 1: call abort(3) */
	/* bit 2: error message be short */
	mallopt(M_CHECK_ACTION, 7);
	
	
	/* Check parent process. */
	parentpid = getppid();
	PRINTDEBUG("Parent PID: %d", parentpid);
	// TODO check ppid owner
	sprintf(pathbuf, "/proc/%d/exe", parentpid);
	pathlen = readlink(pathbuf, parentpath, PATH_MAX-1);
	if(pathlen <= -1)
	{
		/* Can not access parent process. */
		PRINTDEBUG("Can not read parent exe path.");
	}
	else
	{
		/* readlink() does not append a null byte to buf. */
		parentpath[pathlen] = '\0';
		fh = fopen(PARENTSFILE, "r");
		if(fh == NULL)
		{
			cmd = strrchr(parentpath, '/');
			if(cmd == NULL)
			{
				/* Can not find parent command's basename. */
			}
			else
			{
				cmd++;
				PRINTDEBUG("Parent command: %s", cmd);
				if(EQ(cmd, "sshd") || EQ(cmd, "dropbear"))
				{
					/* We are called by SSHd. */
				}
				else
				{
					/* We are not called by SSHd. Bypass access control. */
					PRINTDEBUG("Permissive mode.");
					real_comm = get_real_comm(argv[0], &argv[0]);
					execvpe(real_comm, argv, envp);
					warn("%s", argv[0]);
					return 127;
				}
			}
		}
		else
		{
			/* Read the list of basename/path of possible parent processes. */
			// TODO
		}
	}
	PRINTDEBUG("Controlled mode.");
	
	
	
	group_ids = mallocab(sizeof(gid_t*));
	myuid = getuid();
	pwent = getpwuid(myuid);
	if(pwent == NULL)
	{
		errx(126, "No pw entry for uid %d.", myuid);
	}
	group_ids[0] = pwent->pw_gid;
	
	setgrent();
	n_groups = 1;
	/* Gather group IDs the current user is member of. */
	/* Iterate through system groups. */
	while((grent = getgrent()) != NULL)
	{
		/* Iterate through members of this group. */
		for(n = 0; grent->gr_mem[n] != NULL; n++)
		{
			pwent = getpwnam(grent->gr_mem[n]);
			/* Check if current user is member of this group. */
			if(pwent != NULL && pwent->pw_uid == myuid)
			{
				group_ids = reallocab(group_ids, (n_groups+1) * sizeof(gid_t*));
				group_ids[n_groups] = grent->gr_gid;
				n_groups++;
			}
		}
	}
	PRINTDEBUG("Groups found: %d", n_groups-1);
	endgrent();
	
	
	fh = fopen(CONFFILE, "r");
	if(fh == NULL)
	{
		warn("%s", CONFFILE);
	}
	else
	{
		while(TRUE)
		{
			/* Read a line. */
			fgets(lnbuf, LNBUFLEN, fh);
			if(feof(fh)) break;
			trimtrail(lnbuf, '\n');
			trimtrail(lnbuf, '\r');
			
			/* Take the 1st word (ie. group name). */
			fc_string = strtokdup(lnbuf, 1);
			if(fc_string != NULL && fc_string[0] != '#')
			{
				PARSE_OPT_BOOL(lnbuf, "!report-dotsshrc", &report_dotsshrc);
				PARSE_OPT_BOOL(lnbuf, "!strip-quotes", &strip_quotes);
				
				grent = getgrnam(fc_string);
				FREE(fc_string);
				if(grent != NULL && gid_in_array(grent->gr_gid, group_ids, n_groups))
				{
					boolean allowed = TRUE;
					boolean wildcarded = FALSE;
					boolean invoke_shell;
					int forced_opts_num = -1;
					
					PRINTDEBUG("Reading entry: %s", lnbuf);
					
					/* Current user is a member of this group. */
					/* Check for forced shell options. (optional) */
					for(n = 2; (fc_string = strtokdup(lnbuf, n)) != NULL; n++)
					{
						if(EQ(fc_string, "---"))
						{
							forced_opts_num = n - 2;
							FREE(fc_string);
							break;
						}
						FREE(fc_string);
					}
					if(forced_opts_num == -1) {
						invoke_shell = FALSE;
					} else {
						invoke_shell = TRUE;
					}
					
					/* Assuming argv[1] is "-c". */
					if(argc > 2) {
						cmdline = argv[2];
					} else {
						cmdline = "";
					}
					/* Check if he wants to run an allowed command. */
					for(n = 3 + forced_opts_num; (fc_string = strtokdup(lnbuf, n)) != NULL && (sh_string = strtokdup(cmdline, n - forced_opts_num - 2)) != NULL; n++)
					{
						if(EQ(fc_string, "**")) {
							wildcarded = TRUE;
						} else if(!match(fc_string, sh_string)) {
							allowed = FALSE;
						}
						FREE(fc_string);
						FREE(sh_string);
						if(wildcarded || !allowed) break;
					}
					if(allowed && !wildcarded)
					{
						if((fc_string = strtokdup(lnbuf, n)) != NULL)
						{
							if(!EQ(fc_string, "**"))
							{
								/* More arguments were defined in config file than in command line. */
								allowed = FALSE;
							}
							FREE(fc_string);
						}
						else if((sh_string = strtokdup(cmdline, n - forced_opts_num - 2)) != NULL)
						{
							/* More arguments were given in shell command than in config file. */
							allowed = FALSE;
							FREE(sh_string);
						}
					}
					
					if(allowed)
					{
						fclose(fh);
						real_argv = (char**) mallocab((argc+forced_opts_num+1) * sizeof(char*));
						
						if(invoke_shell)
						{
							PRINTDEBUG("Invoking shell.");
							
							/* Compute command name and 0th argument. */
							real_comm = get_real_comm(argv[0], &real_argv[0]);
							
							/* Append forced options to shell command. */
							for(n = 1; n <= forced_opts_num; n++)
							{
								real_argv[n] = strtokdup(lnbuf, n+1);
							}
							/* Append commandline to shell command. */
							if(argc > 1)
							{
								real_argv[n] = "-c";
								real_argv[n+1] = cmdline;
								real_argv[n+2] = NULL;
							}
							else
							{
								real_argv[n] = NULL;
							}
							/* Forget arguments after: bash -c "..." */
						}
						else
						{
							PRINTDEBUG("Not invoking shell.");
							
							/* Add each words in shell command line as separated arguments. */
							for(n = 0; (sh_string = strtokdup(cmdline, n+1)) != NULL; n++)
							{
								if(strip_quotes && strlen(sh_string)>1 && (sh_string[0]=='\'' && sh_string[strlen(sh_string)-1]=='\'') || (sh_string[0]=='"' && sh_string[strlen(sh_string)-1]=='"'))
								{
									/* Dummy quote-stripper. */
									/* Strip single/douple quotes, but no real interpolation. */
									/* Leave 0th char untouched and refer it as quoting char. */
									unsigned int shift = 0;
									unsigned int pos;
									for(pos = 1; sh_string[pos+shift+1]!=NULL; pos++)
									{
										PRINTDEBUG("unquote [%s]", sh_string);
										PRINTDEBUG("         %*s%s%*s%s", pos, "", shift==0?"↕":"↓", shift==0?0:shift-1, "", shift==0?"":"↑");
										if(sh_string[pos+shift] == '\\' && (sh_string[pos+shift+1] == sh_string[0] || sh_string[pos+shift+1] == '\\'))
										{
											shift++;
										}
										sh_string[pos] = sh_string[pos+shift];
										PRINTDEBUG("unquote [%s]", sh_string);
									}
									sh_string[pos] = '\0';
									PRINTDEBUG("unquoted [%s]", sh_string+1);
									/* Unquoted string starts at char pos 1. */
									real_argv[n] = (char*)sh_string+1;
								}
								else
								{
									real_argv[n] = sh_string;
								}
							}
							real_argv[n] = NULL;
							real_comm = real_argv[0];
						}
						
						PRINTDEBUG("[-]=%s", real_comm);
						for(n=0; real_argv[n]!=NULL; n++)
							PRINTDEBUG("[%d]=%s", n, real_argv[n]);
						
						/* Finally call the desidered command. */
						execvpe(real_comm, real_argv, envp);
						warn("%s", real_argv[0]);
						return 127;
					}
				}
			}
		}
		fclose(fh);
	}
	
	if(report_dotsshrc==FALSE && argc > 2 && (sh_string = strtokdup(argv[2] /* assuming argv[1] is "-c" */, 2 /* assuming 1st token is "/bin/sh" */)) != NULL && EQ(sh_string, ".ssh/rc"))
	{
		/* Don't report that ".ssh/rc" is denied to run. */
	}
	else
	{
		/* Report that shell command is denied to run. */
		if(cmdline == NULL)
		{
			if(argc > 2)
			{
				if(EQ(argv[1], "-c"))
					warnx(MSG_NOCMDLN, argv[2]);
				else
					warnx(MSG_NOWHATE);
			}
			else
				warnx(MSG_NOSHELL);
		}
		else
		{
			if(strlen(cmdline))
				warnx(MSG_NOCMDLN, cmdline);
			else
				warnx(MSG_NOSHELL);
		}
	}
	FREE(sh_string);
	return 1;
}
