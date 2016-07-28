all: ex1 ex3 ex4 ex5 ex6 ex7

# Need to learn how to define bin/ location without explicitly defining every target
ex1:
	cc ex1.c -o bin/ex1

ex3:
	cc ex3.c -o bin/ex3

ex4:
	cc ex4.c -o bin/ex4

ex5:
	cc ex5.c -o bin/ex5

ex6:
	cc ex6.c -o bin/ex6

ex7:
	cc ex7.c -o bin/ex7

###


CFLAGS:-Wall -g

clean:
	-rm -r bin/*.dSYM # Remove *.dSYM/ directories created by valgrind
	-rm bin/*
