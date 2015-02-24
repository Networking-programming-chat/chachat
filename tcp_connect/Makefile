TARGETS = tcp_connect_test

CFLAGS = -g -Wall -Wextra -pthread -std=gnu99

all: $(TARGETS)

tcp_connect_test: tcp_connect_test.c tcp_connect.c

.SUFFIXES : .o .c

.c.o :
	gcc -c -o $@ $(CFLAGS) $<

clean :
	rm *.o $(TARGETS)