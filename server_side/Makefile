TARGETS = main

CFLAGS = -g -Wall -Wextra -pthread -std=gnu99

all: $(TARGETS)

main : main.c serv.c

.SUFFIXES : .o .c

.c.o :
	gcc -c -o $@ $(CFLAGS) $<

clean :
	rm *.o $(TARGETS)