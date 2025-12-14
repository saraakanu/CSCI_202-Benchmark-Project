CC = gcc
CFLAGS = -Wall -Wextra -O2

all: signal signal2 timer

signal: signal.c
	$(CC) $(CFLAGS) -o signal signal.c

signal2: signal2.c
	$(CC) $(CFLAGS) -o signal2 signal2.c

timer: timer.c
	$(CC) $(CFLAGS) -o timer timer.c

clean:
	rm -f signal signal2 timer
