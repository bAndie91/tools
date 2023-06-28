/*
 * last.c	Re-implementation of the 'last' command, this time
 *		for Linux. Yes I know there is BSD last, but I
 *		just felt like writing this. No thanks :-).
 *		Also, this version gives lots more info (especially with -x)
 *
 * Author:	Miquel van Smoorenburg, miquels@cistron.nl
 *
 * Version:	@(#)last  2.85  30-Jul-2004  miquels@cistron.nl
 *
 *		This file is part of the sysvinit suite,
 *		Copyright (C) 1991-2004 Miquel van Smoorenburg.
 *
 *		This program is free software; you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation; either version 2 of the License, or
 *		(at your option) any later version.
 *
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License
 *		along with this program; if not, write to the Free Software
 *		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>
#include <utmp.h>
#include <errno.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "oldutmp.h"

#ifndef SHUTDOWN_TIME
#  define SHUTDOWN_TIME 254
#endif

char *Version = "@(#) last 2.85 31-Apr-2004 miquels";

#define CHOP_DOMAIN	0	/* Define to chop off local domainname. */
#define NEW_UTMP	1	/* Fancy & fast utmp read code. */
#define UCHUNKSIZE	16384	/* How much we read at once. */

/* Double linked list of struct utmp's */
struct utmplist {
  struct utmp ut;
  struct utmplist *next;
  struct utmplist *prev;
};
struct utmplist *utmplist = NULL;

/* Types of listing */
#define R_CRASH		1 /* No logout record, system boot in between */
#define R_DOWN		2 /* System brought down in decent way */
#define R_NORMAL	3 /* Normal */
#define R_NOW		4 /* Still logged in */
#define R_REBOOT	5 /* Reboot record. */
#define R_PHANTOM	6 /* No logout record but session is stale. */
#define R_TIMECHANGE	7 /* NEW_TIME or OLD_TIME */

/* Global variables */
int maxrecs = 0;	/* Maximum number of records to list. */
int recsdone = 0;	/* Number of records listed */
int showhost = 1;	/* Show hostname too? */
int name_len = 8;	/* Default print 8 characters of name */
int domain_len = 16;	/* Default print 16 characters of domain */
int oldfmt = 0;		/* Use old libc5 format? */
char **show = NULL;	/* What do they want us to show */
char *ufile;		/* Filename of this file */
time_t lastdate;	/* Last date we've seen */
char *progname;		/* Name of this program */
#if CHOP_DOMAIN
char hostname[256];	/* For gethostbyname() */
char *domainname;	/* Our domainname. */
#endif

/*
 *	Convert old utmp format to new.
 */
void uconv(struct oldutmp *oldut, struct utmp *utn)
{
	memset(utn, 0, sizeof(struct utmp));
	utn->ut_type = oldut->ut_type;
	utn->ut_pid  = oldut->ut_pid;
	utn->ut_time = oldut->ut_oldtime;
	utn->ut_addr = oldut->ut_oldaddr;
	strncpy(utn->ut_line, oldut->ut_line, OLD_LINESIZE);
	strncpy(utn->ut_user, oldut->ut_user, OLD_NAMESIZE);
	strncpy(utn->ut_host, oldut->ut_host, OLD_HOSTSIZE);
}

#if NEW_UTMP
/*
 *	Read one utmp entry, return in new format.
 *	Automatically reposition file pointer.
 */
int uread(FILE *fp, struct utmp *u, int *quit)
{
	static int utsize;
	static char buf[UCHUNKSIZE];
	char tmp[1024];
	static off_t fpos;
	static int bpos;
	struct oldutmp uto;
	int r;
	off_t o;

	if (quit == NULL && u != NULL) {
		/*
		 *	Normal read.
		 */
		if (oldfmt) {
			r = fread(&uto, sizeof(uto), 1, fp);
			uconv(&uto, u);
		} else
			r = fread(u, sizeof(struct utmp), 1, fp);
		return r;
	}

	if (u == NULL) {
		/*
		 *	Initialize and position.
		 */
		utsize = oldfmt ? sizeof(uto) : sizeof(struct utmp);
		fseeko(fp, 0, SEEK_END);
		fpos = ftello(fp);
		if (fpos == 0)
			return 0;
		o = ((fpos - 1) / UCHUNKSIZE) * UCHUNKSIZE;
		if (fseeko(fp, o, SEEK_SET) < 0) {
			fprintf(stderr, "%s: seek failed!\n", progname);
			return 0;
		}
		bpos = (int)(fpos - o);
		if (fread(buf, bpos, 1, fp) != 1) {
			fprintf(stderr, "%s: read failed!\n", progname);
			return 0;
		}
		fpos = o;
		return 1;
	}

	/*
	 *	Read one struct. From the buffer if possible.
	 */
	bpos -= utsize;
	if (bpos >= 0) {
		if (oldfmt)
			uconv((struct oldutmp *)(buf + bpos), u);
		else
			memcpy(u, buf + bpos, sizeof(struct utmp));
		return 1;
	}

	/*
	 *	Oops we went "below" the buffer. We should be able to
	 *	seek back UCHUNKSIZE bytes.
	 */
	fpos -= UCHUNKSIZE;
	if (fpos < 0)
		return 0;

	/*
	 *	Copy whatever is left in the buffer.
	 */
	memcpy(tmp + (-bpos), buf, utsize + bpos);
	if (fseeko(fp, fpos, SEEK_SET) < 0) {
		perror("fseek");
		return 0;
	}

	/*
	 *	Read another UCHUNKSIZE bytes.
	 */
	if (fread(buf, UCHUNKSIZE, 1, fp) != 1) {
		perror("fread");
		return 0;
	}

	/*
	 *	The end of the UCHUNKSIZE byte buffer should be the first
	 *	few bytes of the current struct utmp.
	 */
	memcpy(tmp, buf + UCHUNKSIZE + bpos, -bpos);
	bpos += UCHUNKSIZE;

	if (oldfmt)
		uconv((struct oldutmp *)tmp, u);
	else
		memcpy(u, tmp, sizeof(struct utmp));

	return 1;
}

#else /* NEW_UTMP */

/*
 *	Read one utmp entry, return in new format.
 *	Automatically reposition file pointer.
 */
int uread(FILE *fp, struct utmp *u, int *quit)
{
	struct oldutmp uto;
	off_t r;

	if (u == NULL) {
		r = oldfmt ? sizeof(struct oldutmp) : sizeof(struct utmp);
		fseek(fp, -1 * r, SEEK_END);
		return 1;
	}

	if (!oldfmt) {
		r = fread(u, sizeof(struct utmp), 1, fp);
		if (r == 1) {
			if (fseeko(fp, -2 * sizeof(struct utmp), SEEK_CUR) < 0)
				if (quit) *quit = 1;
		}
		return r;
	}
	r = fread(&uto, sizeof(struct oldutmp), 1, fp);
	if (r == 1) {
		if (fseeko(fp, -2 * sizeof(struct oldutmp), SEEK_CUR) < 0)
			if (quit) *quit = 1;
		uconv(&uto, u);
	}

	return r;
}
#endif

/*
 *	Try to be smart about the location of the BTMP file
 */
#ifndef BTMP_FILE
#define BTMP_FILE getbtmp()
char *getbtmp()
{
	static char btmp[128];
	char *p;

	strcpy(btmp, WTMP_FILE);
	if ((p = strrchr(btmp, '/')) == NULL)
		p = btmp;
	else
		p++;
	*p = 0;
	strcat(btmp, "btmp");
	return btmp;
}
#endif

/*
 *	Print a short date.
 */
char *showdate()
{
	char *s = ctime(&lastdate);
	s[16] = 0;
	return s;
}

/*
 *	SIGINT handler
 */
void int_handler()
{
	printf("Interrupted %s\n", showdate());
	exit(1);
}

/*
 *	SIGQUIT handler
 */
void quit_handler()
{
	printf("Interrupted %s\n", showdate());
	signal(SIGQUIT, quit_handler);
}

/*
 *	Get the basename of a filename
 */
char *mybasename(char *s)
{
	char *p;

	if ((p = strrchr(s, '/')) != NULL)
		p++;
	else
		p = s;
	return p;
}

char* localtimestamp(time_t time)
{
	char* ret;
	struct tm* loc;
	
	ret = malloc(256);
	loc = localtime(&time);
	if(loc == NULL)
	{
		perror("localtime");
		sprintf(ret, "?");
	}
	else
	{
		strftime(ret, 255, "%s", loc);
	}
	return ret;
	/* FIXME: free(ret) */
}

/*
 *	Show one line of information on screen
 */
int list(struct utmp *p, time_t t, int what)
{
	char		aux1[32] = "";
	char		aux2[32] = "";
	char		ipaddress[16] = "";
	char		final[512];
	char		utline[UT_LINESIZE+1];
	char		domain[256];
	char		*s, **walk;
	int		len;

	/*
	 *	uucp and ftp have special-type entries
	 */
	utline[0] = 0;
	strncat(utline, p->ut_line, UT_LINESIZE);
	if (strncmp(utline, "ftp", 3) == 0 && isdigit(utline[3]))
		utline[3] = 0;
	if (strncmp(utline, "uucp", 4) == 0 && isdigit(utline[4]))
		utline[4] = 0;

	/*
	 *	Is this something we wanna show?
	 */
	if (show) {
		for (walk = show; *walk; walk++) {
			if (strncmp(p->ut_name, *walk, UT_NAMESIZE) == 0 ||
			    strcmp(utline, *walk) == 0 ||
			    (strncmp(utline, "tty", 3) == 0 &&
			     strcmp(utline + 3, *walk) == 0)) break;
		}
		if (*walk == NULL) return 0;
	}

	switch(what) {
		case R_CRASH:
			sprintf(aux2, "crash");
			break;
		case R_DOWN:
			sprintf(aux2, "down");
			break;
		case R_NOW:
			sprintf(aux2, "still logged in");
			break;
		case R_PHANTOM:
			sprintf(aux2, "gone");
			break;
		case R_REBOOT:
			sprintf(aux1, "reboot");
			p->ut_name[0] = 0;
			break;
		case R_TIMECHANGE:
			break;
		case R_NORMAL:
			break;
 	}

	{
		len = UT_HOSTSIZE;
		if (len >= (int)sizeof(domain)) len = sizeof(domain) - 1;
		domain[0] = 0;
		strncat(domain, p->ut_host, len);
	}

#if CHOP_DOMAIN
		/*
		 *	See if this is in our domain.
		 */
		if (s = strchr(p->ut_host, '.') != NULL &&
		     strcmp(s + 1, domainname) == 0) *s = 0;
#endif
			if(p->ut_addr != 0)
				sprintf(ipaddress, "%d.%d.%d.%d", p->ut_addr&0xFF, p->ut_addr>>8&0xFF, p->ut_addr>>16&0xFF, p->ut_addr>>24&0xFF);
			if(p->ut_type == RUN_LVL)
			{
				sprintf(aux1, p->ut_name);
				p->ut_name[0] = 0;
				sprintf(aux2, utline);
				utline[0] = 0;
			}
			else if(p->ut_type == SHUTDOWN_TIME)
			{
				sprintf(aux1, p->ut_name);
				p->ut_name[0] = 0;
			}
			len = snprintf(final, sizeof(final),
				"%s;%s;%s;%s;%s;%s;%s;%s;%d\n",
				p->ut_name, utline, ipaddress, domain, localtimestamp((time_t)p->ut_time), localtimestamp(t), aux1, aux2, p->ut_pid);

#if defined(__GLIBC__)
#  if (__GLIBC__ == 2) && (__GLIBC_MINOR__ == 0)
	final[sizeof(final)-1] = '\0';
#  endif
#endif

	/*
	 *	Print out "final" string safely.
	 */
	for (s = final; *s; s++) {
		if (*s == '\n' || (*s >= 32 && (unsigned char)*s <= 126))
			putchar(*s);
		else
			putchar('*');
	}

	if (len < 0 || (size_t)len >= sizeof(final))
		putchar('\n');

	recsdone++;
	if (maxrecs && recsdone >= maxrecs)
		return 1;

	return 0;
}


/*
 *	show usage
 */
void usage(char *s)
{
	fprintf(stderr, "Usage: %s [-num | -n num] [-f file] "
			"[-t YYYYMMDDHHMMSS] "
			"[-ow] [username..] [tty..]\n", s);
	exit(1);
}

time_t parsetm(char *ts)
{
	struct tm	u, origu;
	time_t		tm;

	memset(&tm, 0, sizeof(tm));

	if (sscanf(ts, "%4d%2d%2d%2d%2d%2d", &u.tm_year,
	    &u.tm_mon, &u.tm_mday, &u.tm_hour, &u.tm_min,
	    &u.tm_sec) != 6)
		return (time_t)-1;

	u.tm_year -= 1900;
	u.tm_mon -= 1;
	u.tm_isdst = -1;

	origu = u;

	if ((tm = mktime(&u)) == (time_t)-1)
		return tm;

	/*
	 *	Unfortunately mktime() is much more forgiving than
	 *	it should be.  For example, it'll gladly accept
	 *	"30" as a valid month number.  This behavior is by
	 *	design, but we don't like it, so we want to detect
	 *	it and complain.
	 */
	if (u.tm_year != origu.tm_year ||
	    u.tm_mon != origu.tm_mon ||
	    u.tm_mday != origu.tm_mday ||
	    u.tm_hour != origu.tm_hour ||
	    u.tm_min != origu.tm_min ||
	    u.tm_sec != origu.tm_sec)
		return (time_t)-1;

	return tm;
}

int main(int argc, char **argv)
{
  FILE *fp;		/* Filepointer of wtmp file */

  struct utmp ut;	/* Current utmp entry */
  struct utmp oldut;	/* Old utmp entry to check for duplicates */
  struct utmplist *p;	/* Pointer into utmplist */
  struct utmplist *next;/* Pointer into utmplist */

  time_t lastboot = 0;  /* Last boottime */
  time_t lastrch = 0;	/* Last run level change */
  time_t lastdown;	/* Last downtime */
  time_t begintime;	/* When wtmp begins */
  int whydown = 0;	/* Why we went down: crash or shutdown */

  int c, x;		/* Scratch */
  struct stat st;	/* To stat the [uw]tmp file */
  int quit = 0;		/* Flag */
  int down = 0;		/* Down flag */
  int lastb = 0;	/* Is this 'lastb' ? */
  char *altufile = NULL;/* Alternate wtmp */

  time_t until = 0;	/* at what time to stop parsing the file */

  progname = mybasename(argv[0]);

  /* Process the arguments. */
  while((c = getopt(argc, argv, "f:n:xot:0123456789w")) != EOF)
    switch(c) {
	case 'n':
		maxrecs = atoi(optarg);
		break;
	case 'o':
		oldfmt = 1;
		break;
	case 'f':
		if((altufile = malloc(strlen(optarg)+1)) == NULL) {
			fprintf(stderr, "%s: out of memory\n",
				progname);
			exit(1);
		}
		strcpy(altufile, optarg);
		break;
	case 't':
		if ((until = parsetm(optarg)) == (time_t)-1) {
			fprintf(stderr, "%s: Invalid time value \"%s\"\n",
				progname, optarg);
			usage(progname);
		}
		break;
	case 'w':
		if (UT_NAMESIZE > name_len)
			name_len = UT_NAMESIZE;
		if (UT_HOSTSIZE > domain_len)
			domain_len = UT_HOSTSIZE;
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		maxrecs = 10*maxrecs + c - '0';
		break;
	default:
		usage(progname);
		break;
    }
  if (optind < argc) show = argv + optind;

  /*
   *	Which file do we want to read?
   */
  if (strncmp(progname, "lastb", 5) == 0) {
	ufile = BTMP_FILE;
	lastb = 1;
  } else
	ufile = WTMP_FILE;
  if (altufile)
	ufile = altufile;
  time(&lastdown);
  lastrch = lastdown;

  /*
   *	Fill in 'lastdate'
   */
  lastdate = lastdown;

#if CHOP_DOMAIN
  /*
   *	Find out domainname.
   *
   *	This doesn't work on modern systems, where only a DNS
   *	lookup of the result from hostname() will get you the domainname.
   *	Remember that domainname() is the NIS domainname, not DNS.
   *	So basically this whole piece of code is bullshit.
   */
  hostname[0] = 0;
  (void) gethostname(hostname, sizeof(hostname));
  if ((domainname = strchr(hostname, '.')) != NULL) domainname++;
  if (domainname == NULL || domainname[0] == 0) {
	hostname[0] = 0;
	(void) getdomainname(hostname, sizeof(hostname));
	hostname[sizeof(hostname) - 1] = 0;
	domainname = hostname;
	if (strcmp(domainname, "(none)") == 0 || domainname[0] == 0)
		domainname = NULL;
  }
#endif

  /*
   *	Install signal handlers
   */
  signal(SIGINT, int_handler);
  signal(SIGQUIT, quit_handler);

  /*
   *	Open the utmp file
   */
  if ((fp = fopen(ufile, "r")) == NULL) {
	x = errno;
	fprintf(stderr, "%s: %s: %s\n", progname, ufile, strerror(errno));
	if (altufile == NULL && x == ENOENT)
		fprintf(stderr, "Perhaps this file was removed by the "
			"operator to prevent logging %s info.\n", progname);
	exit(1);
  }

  /*
   *	Optimize the buffer size.
   */
  setvbuf(fp, NULL, _IOFBF, UCHUNKSIZE);

  /*
   *	Read first structure to capture the time field
   */
  if (uread(fp, &ut, NULL) == 1)
	begintime = ut.ut_time;
  else {
  	fstat(fileno(fp), &st);
	begintime = st.st_ctime;
	quit = 1;
  }

  printf(";;;;%s;;begin;\n", localtimestamp(begintime));


  /*
   *	Go to end of file minus one structure
   *	and/or initialize utmp reading code.
   */
  uread(fp, NULL, NULL);

  /*
   *	Read struct after struct backwards from the file.
   */
  while(!quit) {

	if (uread(fp, &ut, &quit) != 1)
		break;

	if (until && until < ut.ut_time)
		continue;

	if (memcmp(&ut, &oldut, sizeof(struct utmp)) == 0) continue;
	memcpy(&oldut, &ut, sizeof(struct utmp));
	lastdate = ut.ut_time;

  	if (lastb) {
  		quit = list(&ut, ut.ut_time, R_NORMAL);
  		continue;
  	}

	/*
	 *	Set ut_type to the correct type.
	 */
	if (strncmp(ut.ut_line, "~", 1) == 0) {
		strcpy(ut.ut_line, "");
		if (strncmp(ut.ut_user, "shutdown", 8) == 0)
			ut.ut_type = SHUTDOWN_TIME;
		else if (strncmp(ut.ut_user, "reboot", 6) == 0)
			ut.ut_type = BOOT_TIME;
		else if (strncmp(ut.ut_user, "runlevel", 8) == 0)
			ut.ut_type = RUN_LVL;
	}
#if 1 /*def COMPAT*/
	/*
	 *	For stupid old applications that don't fill in
	 *	ut_type correctly.
	 */
	else {
		if (ut.ut_type != DEAD_PROCESS &&
		    ut.ut_name[0] && ut.ut_line[0] &&
		    strcmp(ut.ut_name, "LOGIN") != 0)
			ut.ut_type = USER_PROCESS;
		/*
		 *	Even worse, applications that write ghost
		 *	entries: ut_type set to USER_PROCESS but
		 *	empty ut_name...
		 */
		if (ut.ut_name[0] == 0)
			ut.ut_type = DEAD_PROCESS;

		/*
		 *	Clock changes.
		 */
		if (strcmp(ut.ut_name, "date") == 0) {
			if (ut.ut_line[0] == '|') ut.ut_type = OLD_TIME;
			if (ut.ut_line[0] == '{') ut.ut_type = NEW_TIME;  /* } */
		}
	}
#endif

	switch (ut.ut_type) {
		case SHUTDOWN_TIME:
			quit = list(&ut, lastboot, R_NORMAL);
			lastdown = lastrch = ut.ut_time;
			down = 1;
			break;
		case OLD_TIME:
		case NEW_TIME:
			strcpy(ut.ut_line, ut.ut_type == NEW_TIME ? "new time" : "old time");
			quit = list(&ut, lastdown, R_TIMECHANGE);
			break;
		case BOOT_TIME:
			quit = list(&ut, lastdown, R_REBOOT);
			lastboot = ut.ut_time;
			down = 1;
			break;
		case RUN_LVL:
			x = ut.ut_pid & 255;
				sprintf(ut.ut_line, "%c", x);
				quit = list(&ut, lastrch, R_NORMAL);
			if (x == '0' || x == '6') {
				lastdown = ut.ut_time;
				down = 1;
				ut.ut_type = SHUTDOWN_TIME;
			}
			lastrch = ut.ut_time;
			break;

		case USER_PROCESS:
			/*
			 *	This was a login - show the first matching
			 *	logout record and delete all records with
			 *	the same ut_line.
			 */
			c = 0;
			for (p = utmplist; p; p = next) {
				next = p->next;
				if (strncmp(p->ut.ut_line, ut.ut_line, UT_LINESIZE) == 0) {
					/* Show it */
					if (c == 0) {
						quit = list(&ut, p->ut.ut_time, R_NORMAL);
						c = 1;
					}
					if (p->next) p->next->prev = p->prev;
					if (p->prev)
						p->prev->next = p->next;
					else
						utmplist = p->next;
					free(p);
				}
			}
			/*
			 *	Not found? Then crashed, down, still
			 *	logged in, or missing logout record.
			 */
			if (c == 0) {
				if (lastboot == 0) {
					c = R_NOW;
					/* Is process still alive? */
					if (ut.ut_pid > 0 &&
					    kill(ut.ut_pid, 0) != 0 &&
					    errno == ESRCH)
						c = R_PHANTOM;
				} else
					c = whydown;
				quit = list(&ut, lastboot, c);
			}
			/* FALLTHRU */

		case DEAD_PROCESS:
			/*
			 *	Just store the data if it is
			 *	interesting enough.
			 */
			if (ut.ut_line[0] == 0)
				break;
			if ((p = malloc(sizeof(struct utmplist))) == NULL) {
				fprintf(stderr, "%s: out of memory\n", progname);
				exit(1);
			}
			memcpy(&p->ut, &ut, sizeof(struct utmp));
			p->next  = utmplist;
			p->prev  = NULL;
			if (utmplist) utmplist->prev = p;
			utmplist = p;
			break;

	}
	/*
	 *	If we saw a shutdown/reboot record we can remove
	 *	the entire current utmplist.
	 */
	if (down) {
		lastboot = ut.ut_time;
		whydown = (ut.ut_type == SHUTDOWN_TIME) ? R_DOWN : R_CRASH;
		for (p = utmplist; p; p = next) {
			next = p->next;
			free(p);
		}
		utmplist = NULL;
		down = 0;
	}
  }

  fclose(fp);

  /*
   *	Should we free memory here? Nah. This is not NT :)
   */
  return 0;
}
