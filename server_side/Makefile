PROGS=sqlite3 ccserv dbtest threadtest
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
SQLFLAGS=-Wall -pedantic

all: sqlite ccserv cleanobjects

sqlite:
	$(CC) $(SQLFLAGS) -c sqlite3.c
	ar -cvq libsqlite.a sqlite3.o

ccserv:
	$(CC) -o ccserv $(CFLAGS) test_main.c db.c serv.c netproto.c -pthread -L./ -lsqlite -ldl

dbtest:
	$(CC) -o dbtest $(CFLAGS) db_test_main.c db.c -pthread -L./ -lsqlite -ldl

threadtest:
	$(CC) -o threadtest $(CFLAGS) thread_test_main.c msg_buffers.c -pthread

cleanobjects:
	rm -f *.o

clean:
	rm -f ${PROGS} *~ *.a

.PHONY: dbtest threadtest all clean
