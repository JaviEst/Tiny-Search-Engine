CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -lpthread -pthread 
CC=gcc
LIBS=-lm -lutils -lcurl

all: crawler concurrent_crawler

crawler:	
		$(CC) $(CFLAGS) crawler.c $(LIBS) -o $@

concurrent_crawler:	
		$(CC) $(CFLAGS) concurrent_crawler.c $(LIBS) -o $@

test_crawler_example:
		$(CC) $(CFLAGS) test_crawler_example.c $(LIBS) -o $@

clean:
	rm -rf *.o crawler concurrent_crawler test_crawler_example