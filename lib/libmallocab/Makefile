
LIBDIR = /usr/lib/tool

libmallocab.so: libmallocab.c libmallocab.h
	$(CC) $(CCFLAGS) -o $@ -shared libmallocab.c

$(LIBDIR)/libmallocab.so: libmallocab.so
	install $(notdir $@) $(LIBDIR)/
	@echo remove $@ >> uninstall.sh

install: $(LIBDIR)/libmallocab.so
.PHONY: install
