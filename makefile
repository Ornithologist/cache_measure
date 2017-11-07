CC=gcc
CFLAGS=-g -O3 -fPIC -fno-builtin

all: check

default: check

clean:
	rm -rf *.o size

size: size.o
	$(CC) $(CFLAGS) $< -o $@

gdb: size
	gdb ./size

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

check: clean size
	./size

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar 