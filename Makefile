all: ex1 ex3

ex1:
	cc ex1.c -o bin/ex1

ex3:
	cc ex3.c -o bin/ex3

CFLAGS:-Wall -g

clean:
	rm -f bin/*
