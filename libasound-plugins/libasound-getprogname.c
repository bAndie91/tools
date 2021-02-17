
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


int libasound_helper_remove_unused_mixer_controls();


int snd_func_getprogname(snd_config_t **dst, snd_config_t *root ATTRIBUTE_UNUSED,
		     snd_config_t *src, snd_config_t *private_data ATTRIBUTE_UNUSED)
{
	const char *id;
	char *progname;
	int err = snd_config_get_id(src, &id);
	if (err < 0) return err;
	
	progname = getprogname();
	/* let program softvol controls be on device hw:Loopback,0 */
	/* let audible output softvol controls be on device hw:Loopback,1 */
	int clear_this_loopback_device = 0;
	//libasound_helper_remove_unused_mixer_controls(progname, clear_this_loopback_device);
	
	return snd_config_imake_string(dst, id, progname);
}

SND_DLSYM_BUILD_VERSION(snd_func_getprogname, SND_CONFIG_DLSYM_VERSION_EVALUATE);


int snd_func_findfreeloopback(snd_config_t **dst, snd_config_t *root ATTRIBUTE_UNUSED,
		     snd_config_t *src, snd_config_t *private_data ATTRIBUTE_UNUSED)
{
	const char *id;
	int err = snd_config_get_id(src, &id);
	if (err < 0) return err;
	
	char devname[19];
	char *devname_fixend;
	char lo_idx, lo_subdev;
	snd_pcm_t *spcm;
	/* try to open a Loopback pcm device */
	fprintf(stderr, "snd_func_findfreeloopback: searching for free Loopback device...\n");
	devname_fixend = (char*)(devname+sprintf(devname, "hw:Loopback"));
	for(lo_idx = 0; lo_idx <= 0xFF; lo_idx++)
	{
		for(lo_subdev = 0; lo_subdev < 8; lo_subdev++)
		{
			if(lo_idx == 0) { sprintf(devname_fixend, ",0,%d", lo_subdev); }
			else		    { sprintf(devname_fixend, "_%X,0,%d", lo_idx, lo_subdev); }
			err = snd_pcm_open(&spcm, devname, SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
			//fprintf(stderr, "%s -> error %d\n", devname, err);
			if(err == 0) {
				snd_pcm_close(spcm);
				/* race condition applies here */
				fprintf(stderr, "snd_func_findfreeloopback: found %s\n", devname);
				goto free_device_search_end;
			}
			if(err == -19) {
				fprintf(stderr, "snd_func_findfreeloopback: tried all Loopback devices.\n");
				/* passing on the last (invalid) device to the client to fail on it */
				goto free_device_search_end;
			}
		}
	}
	free_device_search_end:
	
	return snd_config_imake_string(dst, id, devname);
}

SND_DLSYM_BUILD_VERSION(snd_func_findfreeloopback, SND_CONFIG_DLSYM_VERSION_EVALUATE);


//static char card[8] = "default";

int libasound_helper_remove_unused_mixer_controls(const char * except_name, const int remove_from_device)
{
	int err;
	snd_hctl_t *handle;
	snd_hctl_elem_t *elem;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_info_alloca(&info);
	
	char card[8] = "default";
	
	if ((err = snd_hctl_open(&handle, card, 0)) < 0) {
//		error("Control %s open error: %s", card, snd_strerror(err));
		return err;
	}
	if ((err = snd_hctl_load(handle)) < 0) {
//		error("Control %s local error: %s\n", card, snd_strerror(err));
		snd_hctl_close(handle);
		return err;
	}
	snd_ctl_t *ctl;
    if((err = snd_ctl_open(&ctl, card, 0)) < 0) {
//		error("Control %s open error: %s\n", card, snd_strerror(err));
		snd_hctl_close(handle);
		return err;
	}
	char * cname;
	for (elem = snd_hctl_first_elem(handle); elem; elem = snd_hctl_elem_next(elem)) {
		if ((err = snd_hctl_elem_info(elem, info)) < 0) {
//			error("Control %s snd_hctl_elem_info error: %s\n", card, snd_strerror(err));
			continue;
		}
		if (snd_ctl_elem_info_is_inactive(info)) continue;
		if(snd_hctl_elem_get_device(elem) != remove_from_device) continue;
		cname = snd_hctl_elem_get_name(elem);
		if(strcmp(cname, except_name) == 0) continue;
		snd_hctl_elem_get_id(elem, id);
///		if (level & LEVEL_BASIC) show_control("  ", elem, 1);

		//snd_ctl_elem_remove(snd_ctl_t *ctl, snd_ctl_elem_id_t *id)
		snd_ctl_elem_remove(ctl, id);
	}
	snd_ctl_close(ctl);
	snd_hctl_close(handle);
	return 0;
}
