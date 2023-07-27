
LIBDIR = /usr/lib/tool

override CFLAGS += -I../libmallocab -L../libmallocab -lmallocab -Wl,-rpath=$(LIBDIR)


libarray.so: libarray.c libarray.h
	$(CC) $(CFLAGS) -o $@ -shared libarray.c

$(LIBDIR)/libarray.so: libarray.so
	install $(notdir $@) $(LIBDIR)/
	@echo remove $@ >> uninstall.sh

install: $(LIBDIR)/libarray.so
.PHONY: install

clean:
	@rm libarray.so
.PHONY: clean

t_libarray: t_libarray.c
	$(CC) $(CFLAGS) -o $@ t_libarray.c

test: t_libarray
	./t_libarray
.PHONY: test
