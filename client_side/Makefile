TARGETS = client_test

CFLAGS = -g -Wall -Wextra -pthread -std=gnu99

all: $(TARGETS)

client_test: client_test.c client.c

.SUFFIXES : .o .c

.c.o :
	gcc -c -o $@ $(CFLAGS) $<

clean :
	rm *.o $(TARGETS)
