SHELL = /bin/sh

CC=gcc
CFLAGS=-Wall -g -I./networking
LDFLAGS = 
#SQLFLAGS=-Wall -pedantic

NETSRC = $(shell echo ./networking/net*.c)
CLIENTSRC = $(shell echo ./test/testclient/*.c)
SERVERSRC = $(shell echo ./test/testserver/*.c)

all: testserver testclient

testserver:
	$(CC) -o testserver $(CFLAGS) $(LDFLAGS) $(NETSRC) $(SERVERSRC)

testclient:
	$(CC) -o testclient $(CFLAGS) $(LDFLAGS) $(NETSRC) $(CLIENTSRC)

cleanobjects:
	rm -f *.o

clean:
	rm -f testserver testclient
