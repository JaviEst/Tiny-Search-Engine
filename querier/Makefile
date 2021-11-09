CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: query

query:	
		$(CC) $(CFLAGS) query.c $(LIBS) -o $@

clean:
	rm -rf *.o query
