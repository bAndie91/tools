CC = gcc
CFLAGS = -Wall -Wextra -g -I..
LDFLAGS = -L/usr/lib/tool -Wl,-rpath,/usr/lib/tool
T = t_libarray_unit

all: $(T)

$(T): t_libarray_unit.c
	$(CC) $(CFLAGS) t_libarray_unit.c -lmallocab $(LDFLAGS) -o $(T)

clean:
	rm $(T)

run: $(T)
	./$(T)
