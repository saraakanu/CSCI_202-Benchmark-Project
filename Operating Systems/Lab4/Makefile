CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -Wformat
LIBS    = -lpthread

# Default target builds both
all: server client

server: serv.c list.c list.h
	$(CC) $(CFLAGS) serv.c list.c -o server $(LIBS)

client: cli.c
	$(CC) $(CFLAGS) cli.c -o client $(LIBS)

clean:
	rm -f server client *.o
