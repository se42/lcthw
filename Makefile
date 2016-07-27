all: ex1 ex3 ex4

ex1:
	cc ex1.c -o bin/ex1

ex3:
	cc ex3.c -o bin/ex3

ex4:
	cc ex4.c -o bin/ex4

CFLAGS:-Wall -g

clean:
	rm -r bin/*.dSYM # Remove *.dSYM/ directories created by valgrind
	rm bin/*
