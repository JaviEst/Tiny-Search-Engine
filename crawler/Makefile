CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: crawler

crawler:	
		$(CC) $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
	rm -rf *.o crawler