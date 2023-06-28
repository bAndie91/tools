shadow_src = shadow-4.1.5.1

objects = \
 $(shadow_src)/lib/getlong.o \
 $(shadow_src)/lib/getulong.o

$(shadow_src):
	apt-get source shadow

$(shadow_src)/.pc/429_login_FAILLOG_ENAB: | $(shadow_src)
	cd $(shadow_src) && QUILT_PATCHES=debian/patches quilt push -a

$(shadow_src)/Makefile: | $(shadow_src)
	cd $(shadow_src) && ./configure

$(shadow_src)/lib/getlong.o: $(shadow_src)/.pc/429_login_FAILLOG_ENAB $(shadow_src)/Makefile
	$(MAKE) -C $(shadow_src)

logbtmp: logbtmp.c $(shadow_src)/lib/getlong.o
	$(CC) logbtmp.c $(objects) -I$(shadow_src) -o $@
