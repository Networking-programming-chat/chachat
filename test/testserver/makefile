server : test_main.o netproto.o serv.o
	
	cc -o server test_main.o netproto.o serv.o

test_main.o : test_main.c netproto.h serv.h
	
	cc -c test_main.c


netproto.o :netproto.c netproto.h
	
	cc -c netproto.c

serv.o : serv.c serv.h
	
	cc -c serv.c
    
clean :
          
	rm test_main.o netproto.o serv.o server