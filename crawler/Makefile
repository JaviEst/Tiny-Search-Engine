CFLAGS=-Wall -pedantic -std=c11 -I. -L ~/engs50/tse/lib/ -g
CC=gcc
LIBS=-lm -lutils -lcurl

all: 

crawler:	
		$(CC) $(CFLAGS) crawler.c $(LIBS) -o $@

clean:
	rm -rf *.o