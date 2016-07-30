CFLAGS = -Wall -g

all: ex1 ex3 ex4 ex5 ex6 ex7 ex8 ex9 ex10 ex11 ex12 ex13 ex14 ex15

# send all target executables to bin/ directory
ex%:
	cc $(CFLAGS) $@.c -o bin/$@

clean:
	-rm -r bin/*.dSYM
	-rm bin/*
