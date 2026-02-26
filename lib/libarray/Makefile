
LIBDIR = /usr/lib/tool
OLDINCLUDEDIR = /usr/include/tool

override CFLAGS += -L$(LIBDIR) -Wl,-rpath=$(LIBDIR) -lmallocab


libarray.so: libarray.c libarray.h
	$(CC) $< $(CFLAGS) -I. -o $@ -shared

$(LIBDIR)/libarray.so: libarray.so
	install -m 0644 $(notdir $@) $(LIBDIR)/
	@echo remove $@ >> uninstall.sh

$(OLDINCLUDEDIR)/libarray.h: libarray.h | $(OLDINCLUDEDIR)
	install -m 0644 $(notdir $@) $(OLDINCLUDEDIR)/
	@echo remove $@ >> uninstall.sh

$(OLDINCLUDEDIR):
	[ -d $(OLDINCLUDEDIR) ] || mkdir $(OLDINCLUDEDIR)
	@echo remove $@ >> uninstall.sh

install: $(LIBDIR)/libarray.so $(OLDINCLUDEDIR)/libarray.h
	ldconfig
.PHONY: install

clean:
	@rm libarray.so t_libarray
.PHONY: clean

t_libarray: t_libarray.c libarray.so
	$(CC) $< $(CFLAGS) -I. -L. -larray -Wl,-rpath=. -o $@

test: t_libarray
	./t_libarray
.PHONY: test

unit-tests:
	$(MAKE) -C ./tests run
.PHONY: unit-tests
