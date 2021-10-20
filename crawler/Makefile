CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: crawler test_crawler_example

crawler:	
		$(CC) $(CFLAGS) crawler.c $(LIBS) -o $@

test_crawler_example:
		$(CC) $(CFLAGS) test_crawler_example.c $(LIBS) -o $@

clean:
	rm -rf *.o crawler test_crawler_example