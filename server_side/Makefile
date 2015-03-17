PROGS=sqlite3 ccserv
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
SQLFLAGS=-Wall -pedantic

all: clean sqlite ccserv cleanobjects

sqlite:
	$(CC) $(SQLFLAGS) -c sqlite3.c
	ar -cvq libsqlite.a sqlite3.o

ccserv:
	$(CC) -o ccserv $(CFLAGS) test_main.c serv.c netproto.c -pthread -L./ -lsqlite

cleanobjects:
	rm -f *.o

clean:
	rm -f ${PROGS} *~ *.a
