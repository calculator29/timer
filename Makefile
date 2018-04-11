CC=gcc
CFLAGS=-Wall -O3 $(LIB) $(INCLUDE)

all: timer

timer: include/timer.h src/timer.c
	$(CC) $(CFLAGS) -c src/timer.c -o lib/timer.o

clean:
	rm lib/timer.o
