
sh.ssh: libmallocab.so libstrtokdup.so
	gcc $(DEBUG) $(CCFLAGS) -o $@ sh.ssh.c $+

debug: DEBUG = -DDEBUG

debug: sh.ssh

libmallocab.so:
	gcc $(DEBUG) $(CCFLAGS) -o $@ -shared libmallocab.c

libstrtokdup.so:
	gcc $(DEBUG) $(CCFLAGS) -o $@ -shared libstrtokdup.c

t_strtokdup: libmallocab.so libstrtokdup.so
	gcc $(DEBUG) $(CCFLAGS) -o $@ t_strtokdup.c $+

install: sh.ssh
	install sh.ssh -m 0755 /usr/bin/sh.ssh
	ln -sfT sh.ssh /usr/bin/bash.ssh
	ln -sfT bash.ssh /usr/bin/rbash.ssh
