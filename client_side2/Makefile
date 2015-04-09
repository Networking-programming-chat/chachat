PROGS=clienttest
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L -I../networking
SQLFLAGS=-Wall 


all: clienttest cleanobjects 

clienttest:
	$(CC) -o clienttest $(CFLAGS) test_client_main.c ../networking/netproto.c client.c -pthread
cleanobjects:
	rm -f *.o

clean:
	rm -f ${PROGS} *~ *.a
