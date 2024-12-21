
autossl.so: autossl.c
	gcc $(CCFLAGS) $< -D_GNU_SOURCE -ldl -lbsd -shared -fPIC -o $@
	chmod -x $@

install: /usr/tool/autossl /usr/tool/autossl-conn /usr/lib/tool/preload/autossl.so manpages
.PHONY: install

autossl autossl-conn autossl.pod autossl.so.pod:
	@true

/usr/tool/autossl: autossl
	install $< $@

/usr/tool/autossl-conn: autossl-conn
	install $< $@

/usr/lib/tool/preload/autossl.so: autossl.so | /usr/lib/tool/preload
	install $< $@

/usr/lib/tool/preload:
	mkdir -p $@

manpages: /usr/share/man/man1/autossl.1.xz /usr/share/man/man8/autossl.so.8.xz
	/etc/cron.daily/man-db
.PHONY: manpages

/usr/share/man/man1/autossl.1.xz: autossl.pod
	pod2man --name=autossl --section 1 --utf8 $< | xz > $@.tmp && \
	mv $@.tmp $@

/usr/share/man/man8/autossl.so.8.xz: autossl.so.pod
	pod2man --name=autossl.so --section 8 --utf8 $< | xz > $@.tmp && \
	mv $@.tmp $@

define rid
  @[ ! -e $(1) ] || rm -v $(1)
endef

uninstall:
	$(call rid,/usr/tool/autossl)
	$(call rid,/usr/tool/autossl-conn)
	$(call rid,/usr/lib/tool/preload/autossl.so)
	$(call rid,/usr/share/man/man1/autossl.1.xz.tmp)
	$(call rid,/usr/share/man/man1/autossl.1.xz)
	$(call rid,/usr/share/man/man8/autossl.so.8.xz.tmp)
	$(call rid,/usr/share/man/man8/autossl.so.8.xz)
.PHONY: uninstall
