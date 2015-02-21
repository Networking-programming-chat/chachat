PROGS=sqlite3 database
CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L
SQLFLAGS=-Wall -pedantic

all: clean sqlite db cleanobjects

sqlite:
	$(CC) $(SQLFLAGS) -c sqlite3.c
	ar -cvq libsqlite.a sqlite3.o

db:
	$(CC) -o database $(CFLAGS) database.c -L./ -lsqlite

cleanobjects:
	rm -f *.o

clean:
	rm -f ${PROGS} *~ *.a
