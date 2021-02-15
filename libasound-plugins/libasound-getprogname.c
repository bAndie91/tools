
/*
DESCRIPTION

	This libasound (ALSA) configuration plugin provides "getprogname" @func function.
	It enables you to have eg. a softvolume control channel for each program.
	When a new control is created, the client program have to stop+play or restart,
	otherwise the control's volume level can not be changed. Tough, it works great at
	the second time.
	The created control stays there after the program exists.

EXAMPLE

	~/.asoundrc:

	pcm.softvol0 {
		type softvol
		slave.pcm "hw:0"
		control {
			name {
				@func getprogname
				default "SoftVolume0"
			}
			card hw
			device 0
		}
	}
	
	func.getprogname {
		lib "/usr/src/alsa-lib-1.0.25/libasound-getprogname.so"
		func "snd_func_getprogname"
	}

BUILD

	Compile against alsa-lib-1.0.25
	
	if libtool acts weirdly, it may forgot that $ECHO is $echo ... or the other way around...
		
	
	# not good
	#gcc -Iinclude/ -lbsd libasound-getprogname.c -o libasound-getprogname.so -shared -fPIC -Wl,--version-script=libasound-getprogname.ver
	
	# less wrong? it worked anyway...
	mkdir .libs
	./libtool --tag=CC   --mode=compile gcc -DHAVE_CONFIG_H -I. -Wall -g -I./include/  \
		-D_GNU_SOURCE -g -O2  -MD -MP  -c -o libasound-getprogname.lo libasound-getprogname.c
	gcc -shared -lbsd -lasound .libs/libasound-getprogname.o -o libasound-getprogname.so
*/

#include <bsd/stdlib.h>

#include "include/local.h"

int snd_func_getprogname(snd_config_t **dst, snd_config_t *root ATTRIBUTE_UNUSED,
		     snd_config_t *src, snd_config_t *private_data ATTRIBUTE_UNUSED)
{
	const char *id;
	int err = snd_config_get_id(src, &id);
	if (err < 0) return err;
	
	char *progname = getprogname();
	return snd_config_imake_string(dst, id, progname);
}

SND_DLSYM_BUILD_VERSION(snd_func_getprogname, SND_CONFIG_DLSYM_VERSION_EVALUATE);
