PROGS=sqlite3 ccserv dbtest threadtest servertest
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L -I../networking
SQLFLAGS=-Wall 

NETSRC = $(shell echo ../networking/net*.c)

all: sqlite servtest cleanobjects 

sqlite:
	$(CC) $(SQLFLAGS) -c sqlite3.c
	ar -cvq libsqlite.a sqlite3.o

dbtest:
	$(CC) -o dbtest $(CFLAGS) db_test_main.c db.c -pthread -L./ -lsqlite -ldl

threadtest:
	$(CC) -o threadtest $(CFLAGS) thread_test_main.c msg_buffers.c -pthread

servtest:
	$(CC) -o servertest $(CFLAGS) servermain.c db.c serv.c $(NETSRC) msg_buffers.c -pthread -L./ -lsqlite -ldl

cleanobjects:
	rm -f *.o

clean: cleanobjects
	rm -f ${PROGS} *~ *.a *.db

.PHONY: dbtest threadtest all clean
