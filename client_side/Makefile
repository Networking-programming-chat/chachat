PROGS= client
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
all: clean client cleanobjects

client:
	$(CC) -o client $(CFLAGS) main.c client.c client_connect.c client_nick.c chatMsg.c chanMsg.c printRule.c quitMsg.c ../networking/netproto.c
		
cleanobjects:
	rm -f *.o
		
clean:
	rm -f ${PROGS} *~ *.a
