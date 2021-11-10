CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g -lpthread
CC=gcc
LIBS=-lm -lutils -lcurl

# get all c files whose name starts test. This is used to extract the target name
#https://stackoverflow.com/questions/13995906/get-filename-without-extension-within-makefile
TEST_LIST_CFILES:=$(wildcard test_list*.c)
LIST_TARGETS:=$(patsubst %.c,%,$(TEST_LIST_CFILES))
LIST_OBJS:=$(patsubst %.c,%.o,$(TEST_LIST_CFILES))

TEST_HASH_CFILES:=$(wildcard test_hash*.c)
HASH_TARGETS:=$(patsubst %.c,%,$(TEST_HASH_CFILES))
HASH_OBJS:=$(patsubst %.c,%.o,$(TEST_HASH_CFILES))

TEST_QUEUE_CFILES:=$(wildcard test_queue*.c)
QUEUE_TARGETS:=$(patsubst %.c,%,$(TEST_QUEUE_CFILES))
QUEUE_OBJS:=$(patsubst %.c,%.o,$(TEST_QUEUE_CFILES))

TEST_LQUEUE_CFILES:=$(wildcard test_lqueue*.c)
LQUEUE_TARGETS:=$(patsubst %.c,%,$(TEST_LQUEUE_CFILES))
LQUEUE_OBJS:=$(patsubst %.c,%.o,$(TEST_LQUEUE_CFILES))

TEST_LHASH_CFILES:=$(wildcard test_lhash*.c)
LHASH_TARGETS:=$(patsubst %.c,%,$(TEST_LHASH_CFILES))
LHASH_OBJS:=$(patsubst %.c,%.o,$(TEST_LHASH_CFILES))


OFILES=queue.o hash.o webpage.o pageio.o indexio.o query_indexer.o lqueue.o lhash.o

all: $(OFILES)
	 ar cr ../lib/libutils.a $(OFILES)
	 
# all: $(LQUEUE_TARGETS) # $(LHASH_TARGETS) # test_indexio test_pageio_load $(LIST_TARGETS) $(HASH_TARGETS) $(QUEUE_TARGETS)

$(LIST_TARGETS) : %: %.o list.o
	$(CC) $(CFLAGS) $< list.o -o $@ 

$(QUEUE_TARGETS) : %: %.o queue.o
	$(CC) $(CFLAGS) $< queue.o -o $@ 

$(LQUEUE_TARGETS) : %: %.o lqueue.o
	$(CC) $(CFLAGS) $< lqueue.o -o $@ 

$(HASH_TARGETS) : %: %.o hash.o queue.o
	$(CC) $(CFLAGS) $< hash.o queue.o -o $@

$(LHASH_TARGETS) : %: %.o lhash.o queue.o
	$(CC) $(CFLAGS) $< lhash.o queue.o -o $@


$(LIST_OBJS) : %.o: %.c list.h
	$(CC) $(CFLAGS) -c $< 

$(QUEUE_OBJS) : %.o: %.c queue.h
	$(CC) $(CFLAGS) -c $< 

$(LQUEUE_OBJS) : %.o: %.c lqueue.h
	$(CC) $(CFLAGS) -c $< 

$(HASH_OBJS) : %.o: %.c hash.h queue.h
	$(CC) $(CFLAGS) -c $< 

$(LHASH_OBJS) : %.o: %.c lhash.h queue.h
	$(CC) $(CFLAGS) -c $< 


query_indexer.o: query_indexer.c
				 gcc $(CFLAGS) -c query_indexer.c

webpage.o:	webpage.c webpage.h
			gcc $(CFLAGS) -c webpage.c

pageio.o:	pageio.c pageio.h
			gcc $(CFLAGS) -c pageio.c

indexio.o:	indexio.c indexio.h
			gcc $(CFLAGS) -c indexio.c

test_pageio_load:	
		$(CC) $(CFLAGS) test_pageio_load.c $(LIBS) -o $@

test_indexio:	
		$(CC) $(CFLAGS) test_indexio.c $(LIBS) -o $@


clean:
	rm -rf *.o test_indexio test_pageio_load $(LIST_TARGETS) $(QUEUE_TARGETS) $(LQUEUE_TARGETS) $(HASH_TARGETS) $(LHASH_TARGETS)