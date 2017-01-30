
sh.ssh: libmallocab.so libstrtokdup.so
	gcc -o $@ sh.ssh.c $+

libmallocab.so:
	gcc -o $@ -shared libmallocab.c

libstrtokdup.so:
	gcc -o $@ -shared libstrtokdup.c

t_strtokdup: libmallocab.so libstrtokdup.so
	gcc -o $@ t_strtokdup.c $+
