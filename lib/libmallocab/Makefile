
LIBDIR = /usr/lib/tool
OLDINCLUDEDIR = /usr/include

libmallocab.so: libmallocab.c libmallocab.h
	$(CC) $(CCFLAGS) -o $@ -shared libmallocab.c

$(LIBDIR)/libmallocab.so: libmallocab.so
	install -m 0644 $(notdir $@) $(LIBDIR)/
	@echo remove $@ >> uninstall.sh

$(OLDINCLUDEDIR)/libmallocab.h: libmallocab.h
	install -m 0644 $(notdir $@) $(OLDINCLUDEDIR)/
	@echo remove $@ >> uninstall.sh

install: $(LIBDIR)/libmallocab.so $(OLDINCLUDEDIR)/libmallocab.h
	ldconfig
.PHONY: install
