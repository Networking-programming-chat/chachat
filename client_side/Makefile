PROGS= client clientmain
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
all: clean client cleanobjects

client:
	$(CC) -o client $(CFLAGS) main.c client.c client_connect.c client_nick.c chatMsg.c chanMsg.c printRule.c -pthread -L quitMsg.c ../networking/netproto.c
		
cleanobjects:
	rm -f *.o

clienttest:
	$(CC) -o clientmain $(CFLAGS) test_client_main.c

clean:
	rm -f ${PROGS} *~ *.a
