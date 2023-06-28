/* 
 * reaper.c: A process capable of killing `runner`
 *
 * Copyright (C) 2018 Citrix Systems UK Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#define _GNU_SOURCE
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <err.h>

int main(int argc, char ** argv) {
	argc--;
	if (argc != 2)
	{
		errx(EXIT_FAILURE, "Usage: kill-rogue <rogue-uid> <reaper-uid>");
	}
    uid_t euid;
    uid_t tuid = atoi(argv[1]);
    uid_t xuid = atoi(argv[2]);
    int rc;
    
    if (tuid == 0 || xuid == 0)
    {
    	errx(EXIT_FAILURE, "rouge-uid or reaper-uid is 0, which should not");
    }

    /* Check to make sure we have enough permissions */
    euid = geteuid();

    if ( euid != 0 ) {
        errx(EXIT_FAILURE, "Must run as euid 0 to set uid");
    }

    /* 
     * Set euid to TARGET_UID so that we can kill the rogue 'runner'; but
     * set ruid to REAPER_UID so that the rogue runner can't kill us.
     */
    if ( setresuid(xuid, tuid, 0) ) {
        err(EXIT_FAILURE, "Setting uid to target");
    }

    rc = kill(-1, 9);
    if ( rc )
        err(EXIT_FAILURE, "No processes killed");
    else
        warnx("At least one process killed successfully");

    return EXIT_SUCCESS;
}
