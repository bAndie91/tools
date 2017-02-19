
sh.ssh: libmallocab.so libstrtokdup.so
	gcc -o $@ sh.ssh.c $+

libmallocab.so:
	gcc -o $@ -shared libmallocab.c

libstrtokdup.so:
	gcc -o $@ -shared libstrtokdup.c

t_strtokdup: libmallocab.so libstrtokdup.so
	gcc -o $@ t_strtokdup.c $+

install: sh.ssh
	install sh.ssh -m 0755 /usr/bin/sh.ssh
	ln -sfT sh.ssh /usr/bin/bash.ssh
	ln -sfT bash.ssh /usr/bin/rbash.ssh
