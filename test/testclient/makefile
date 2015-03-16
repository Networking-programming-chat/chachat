client : main.o netproto.o
	
	cc -o client main.o netproto.o

main.o : main.c netproto.h
	
	cc -c main.c


netproto.o :netproto.c netproto.h
	
	cc -c netproto.c
    
clean :
          
	rm client main.o netproto.o