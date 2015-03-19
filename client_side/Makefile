PROGS= client
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
all: clean client cleanobjects
client:
$(CC) -o client $(CFLAGS) client_test.c client.c netproto.c 
cleanobjects:
rm -f *.o
clean:
rm -f ${PROGS} *~ *.a
