all: get put
	chmod u+s get put

debug: get.c put.c lib.c
	cc -o put put.c lib.c -g
	cc -o get get.c lib.c -g
	chmod u+s get put

put: put.o lib.o
	cc -o put put.o lib.o

get: get.o lib.o
	cc -o get get.o lib.o

put.o: put.c
	cc -c -o put.o put.c

get.o: get.c
	cc -c -o get.o get.c

lib.o: lib.c
	cc -c -o lib.o lib.c

.PHONY: clean
clean:
	rm -rf get put *.o *.dSYM
