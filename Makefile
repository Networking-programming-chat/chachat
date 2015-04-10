SHELL = /bin/sh

CC=gcc
CFLAGS=-Wall -pedantic -g -std=c99 -D_POSIX_C_SOURCE=200112L -I./server_side -pthread
LDFLAGS = -L./lib/ -lsqlite -ldl
SQLFLAGS=-Wall

LIB=./lib/
SERVERSRCPATH=./server_side/

NETSRC = $(shell echo ./networking/net*.c)
CLIENTSRC = $(shell echo ./client_side2/*.c)
SERVERSRC = $(SERVERSRCPATH)servermain.c $(SERVERSRCPATH)db.c $(SERVERSRCPATH)serv.c $(SERVERSRCPATH)msg_buffers.c
SQLSRC = $(shell echo ./server_side/*sql*.c)
OBJECTS=$(shell echo $(LIB)*.o)

CLIENT=client
SERVER=chachat-server

PROGS=$(CLIENT) $(SERVER)

all: $(SERVER) $(CLIENT) tester cleanobjects

$(SERVER):
	mkdir -p bin; \
	cd server_side; \
	make; \
	mv servertest ../bin/$(SERVER)

$(CLIENT):
	mkdir -p bin; \
	cd client_side2; \
	make; \
	mv clienttest ../bin/$(CLIENT)

tester: 
	mkdir -p bin; \
	cd test/testserver; \
	make; \
	mv a.out ../../bin/tester

cleanobjects:
	rm -f ./server_side/*.o ./server_side/*.a

clean: cleanobjects
	rm -fr bin *~ ./server_side/*.a ./server_side/*.db

.PHONY: dbtest threadtest all clean
