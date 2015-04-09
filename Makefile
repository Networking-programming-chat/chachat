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

all: $(SERVER) $(CLIENT) cleanobjects

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

cleanobjects:
	rm -f $(SERVERSRCPATH)*.o *.a

clean: cleanobjects
	rm -fr bin *~ *.a *.db

.PHONY: dbtest threadtest all clean
