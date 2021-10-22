CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: indexer 

indexer:	
		$(CC) $(CFLAGS) indexer.c $(LIBS) -o $@

clean:
	rm -rf *.o indexer