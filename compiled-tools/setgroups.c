/*
  Compile-time config options:
    ONLY_ALLOWED_GID
      If you set a gid, then what 'setgroups' does is only add this
      group to the supplementary group list. So you call it like:
      
      ./setgroups some command
      
      If you don't set, then you have to pass group IDs you wish to
      take on, and command argument you wish to execute with the new
      supplementary group set, separated by '--', like:
      
      ./setgroups 199 1000 4256 -- some command
  
  Set file capabilities after compiled:
    setcaps cap_setgid+ep ./setgroups
 */

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <string.h>


int main(int argc, char ** argv)
{
	gid_t * gidlist;
	int sz_gidlist;
	int aux1;
	int aux2;
	int backshift;
	int n_groups;
	char ** endptr;
	int n_set_groups;
	int n_removed_groups;
	long set_gid;
	char ** execargs;
	
	if(argc < 2) errx(-1, "No command given.");
	
	n_removed_groups = 0;
	sz_gidlist = 4;
	gidlist = malloc(sz_gidlist * sizeof(gid_t));
	if(gidlist == NULL) abort();
	
	while((n_groups = getgroups(sz_gidlist, gidlist)) == -1)
	{
		if(errno == EINVAL)
		{
			sz_gidlist *= 2;
			gidlist = realloc(gidlist, sz_gidlist * sizeof(gid_t));
			if(gidlist == NULL) abort();
		}
		else
		{
			err(errno, "getgroups");
		}
	}
	
#ifndef ONLY_ALLOWED_GID
	n_set_groups = 0;
	
	for(aux1 = 1; aux1 < argc; aux1++)
	{
		if(strcmp(argv[aux1], "--") == 0) { aux1++; break; }
		n_set_groups += 1;
	}
	
	if(aux1 >= argc) errx(-1, "No command given.");
	execargs = &argv[aux1];
#else
	n_set_groups = 1;
	execargs = &argv[1];
#endif

	if(n_groups + n_set_groups /* TODO: don't allocate memory for gids which will be removed */ > sz_gidlist)
	{
		sz_gidlist += n_set_groups;
		gidlist = realloc(gidlist, sz_gidlist * sizeof(gid_t));
		if(gidlist == NULL) abort();
	}
	
#ifndef ONLY_ALLOWED_GID
	for(aux1 = 0; aux1 < n_set_groups; aux1++)
	{
		errno = 0;
		set_gid = strtol(argv[1+aux1], endptr, 0);
		if(*endptr == argv[1+aux1] || **endptr != '\0') errx(-1, "Can not parse '%s' as GID", argv[1+aux1]);
		if(errno != 0) err(errno, "Can not parse '%s' as GID", argv[1+aux1]);
		
		if(set_gid > 0)
		{
			gidlist[n_groups - 2*n_removed_groups + aux1] = (gid_t)set_gid;
		}
		else
		{
			backshift = 0;
			for(aux2 = 0; aux2 < n_groups - n_removed_groups - backshift + aux1; aux2++)
			{
				if(gidlist[aux2] == -set_gid) backshift += 1;
				gidlist[aux2] = gidlist[aux2 + backshift];
			}
			n_removed_groups += backshift;
		}
	}
#else
	gidlist[n_groups] = ONLY_ALLOWED_GID;
#endif
	
	if(setgroups(n_groups + n_set_groups - 2*n_removed_groups, gidlist) != 0)
	{
		err(errno, "setgroups");
	}
	
	execvp(execargs[0], execargs);
	err(errno, "exec: %s", execargs[0]);
}
