CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lSDL2

test: test.o chip.o
	$(CC) $(CFLAGS) -o test test.o chip.o $(LDFLAGS)

test.o: test.c
	$(CC) $(CFLAGS) -c test.c

chip.o: chip.c
	$(CC) $(CFLAGS) -c chip.c

clean:
	rm -f *.o test