all: get put

put: put.o lib.o
	clang -o put put.o lib.o

get: get.o lib.o
	clang -o get get.o lib.o

put.o: put.c
	clang -c -o put.o put.c

get.o: get.c
	clang -c -o get.o get.c

lib.o: lib.c
	clang -c -o lib.o lib.c

.PHONY: clean
clean:
	rm -rf get put *.o
