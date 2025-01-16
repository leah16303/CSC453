CC = gcc
CFLAGS = -Wall -g -fPIC -Wno-deprecated-declarations

intel-all: lib/libmalloc.so lib64/libmalloc.so

lib/libmalloc.so: lib malloc32.o
	$(CC) $(CFLAGS) -m32 -shared -o $@ malloc32.o

lib64/libmalloc.so: lib64 malloc64.o
	$(CC) $(CFLAGS) -shared -o $@ malloc64.o

lib:
	mkdir -p lib

lib64:
	mkdir -p lib64

malloc32.o: malloc.c
	$(CC) $(CFLAGS) -m32 -c -o malloc32.o malloc.c

malloc64.o: malloc.c
		$(CC) $(CFLAGS) -m64 -c -o malloc64.o malloc.c

lib64/libmalloc.a: lib malloc64.o
	ar r libmalloc.a malloc64.o

prog32: malloc_tests.c intel-all lib/libmalloc.so
	$(CC) $(CFLAGS) -o prog32 malloc_tests.c -L ./lib -l malloc

prog64: malloc_tests.c intel-all lib/libmalloc.so
	$(CC) $(CFLAGS)	-o prog malloc_tests.c -L ./lib64 -l malloc

test64: malloc_tests.c lib64/libmalloc.so
	$(CC) $(CFLAGS) malloc_tests.c -L ./lib64 -l malloc -o test64

test32: malloc_tests.c lib/libmalloc.so
	$(CC) $(CFLAGS) malloc_tests.c -L ./lib -l malloc -o test32

clean:
	rm -f core.* malloc.o *~ TAGS

malloc: lib64/libmalloc.so lib64/libmalloc.a
	@echo "libmalloc.so is ready for use."