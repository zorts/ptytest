
CC = c99

all: ptytest

ptytest: ptytest.c
	${CC} -o $@ $< 

clean:
	rm -rf *.o ptytest
