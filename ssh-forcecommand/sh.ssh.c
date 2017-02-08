
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

#include "libmallocab.h"
#include "libstrtokdup.h"

#define EQ(a,b) (strcmp(a,b)==0)
#define FREE(p) {if(p!=NULL){free(p);p=NULL;}}while(0)
typedef int boolean;
#define FALSE 0
#define TRUE !FALSE


#define CONFFILE "/etc/ssh/AllowGroupCommands"
#define SUPPRESS_MSG_DOTSSHRC 0



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
	char lnbuf[4096];
	char* fc_string = NULL;
	char* sh_string = NULL;
	char* cmdline;
	
	
	mallopt(M_CHECK_ACTION, 7);
	
	
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
	/* Iterate through system groups */
	while((grent = getgrent()) != NULL)
	{
		/* Iterate through members of this group */
		for(n = 0; grent->gr_mem[n] != NULL; n++)
		{
			pwent = getpwnam(grent->gr_mem[n]);
			/* Check if current user is member of this group */
			if(pwent != NULL && pwent->pw_uid == myuid)
			{
				group_ids = reallocab(group_ids, (n_groups+1) * sizeof(gid_t*));
				group_ids[n_groups] = grent->gr_gid;
				n_groups++;
			}
		}
	}
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
			/* Read a line */
			fgets(lnbuf, sizeof(lnbuf), fh);
			if(feof(fh)) break;
			
			/* Take the 1st word (ie. group name) */
			fc_string = strtokdup(lnbuf, 1);
			if(fc_string != NULL && fc_string[0] != '#')
			{
				grent = getgrnam(fc_string);
				FREE(fc_string);
				if(grent != NULL && gid_in_array(grent->gr_gid, group_ids, n_groups))
				{
					boolean allowed = TRUE;
					boolean wildcarded = FALSE;
					boolean invoke_shell;
					int forced_opts_num = -1;
					
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
					
					/* Assuming argv[1] == "-c" */
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
							/* Compose command name */
							char* cp = strrchr(argv[0], '/');
							if(cp == NULL)
							{
								/* Take our basename. */
								real_argv[0] = argv[0];
							}
							else
							{
								/* Point to our basename. */
								real_argv[0] = cp+1;
							}
							/* Strip everything after the last dot, if there is one. */
							cp = real_argv[0];
							if(strrchr(cp, '.') != NULL)
							{
								real_argv[0] = strndupab(cp, strrchr(cp, '.') - cp);
							}
							/* If our name starts with a dash, let command do not. */
							if(real_argv[0][0] == '-')
							{
								real_comm = &real_argv[0][1];
							}
							else
							{
								real_comm = real_argv[0];
							}
							
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
							else real_argv[n] = NULL;
						}
						else
						{
							for(n = 0; (sh_string = strtokdup(cmdline, n+1)) != NULL; n++)
							{
								real_argv[n] = sh_string;
							}
							real_argv[n] = NULL;
							real_comm = real_argv[0];
						}
						
						// DEBUG // fprintf(stderr, "%s\n", real_comm); for(n=0; real_argv[n]!=NULL; n++) fprintf(stderr, "[%d]=%s\n", n, real_argv[n]);
						execvpe(real_comm, real_argv, envp);
						warn("%s", real_argv[0]);
						return 127;
					}
				}
			}
		}
		fclose(fh);
	}
	
#if SUPPRESS_MSG_DOTSSHRC
	if(argc > 2 && (sh_string = strtokdup(cmdline, 2)) != NULL && EQ(sh_string, ".ssh/rc"))
		/* Don't report that ".ssh/rc" is denied to run. */{}
	else
#endif
		if(strlen(cmdline))
			warnx("Not allowed to run requested command line: %s", cmdline);
		else
			warnx("Not allowed to run interactive shell.");
	FREE(sh_string);
	return 1;
}
