
libarray.so: override CFLAGS += -I../libmallocab -L../libmallocab -lmallocab

libarray.so: libarray.c libarray.h
	$(CC) $(CFLAGS) -o $@ -shared libarray.c
