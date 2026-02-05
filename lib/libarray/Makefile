
LIBDIR = /usr/lib/tool
OLDINCLUDEDIR = /usr/include/tool

override CFLAGS += -lmallocab -L/usr/lib/tool -Wl,-rpath=$(LIBDIR)


libarray.so: libarray.c libarray.h
	$(CC) $(CFLAGS) -o $@ -shared libarray.c

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

t_libarray: t_libarray.c
	$(CC) $(CFLAGS) -o $@ t_libarray.c

test: t_libarray
	./t_libarray
.PHONY: test
