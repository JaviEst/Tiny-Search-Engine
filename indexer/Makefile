CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: indexer indexer_step2

indexer:	
		$(CC) $(CFLAGS) indexer.c $(LIBS) -o $@

indexer_step2:	
		$(CC) $(CFLAGS) indexer_step2.c $(LIBS) -o $@

clean:
	rm -rf *.o indexer indexer_step2