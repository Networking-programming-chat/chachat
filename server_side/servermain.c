//
//  main.c
//  chachat1
//
//  Created by Judy on 3/13/15.
//  Copyright (c) 2015 Judy. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h> // malloc, free
#include <unistd.h> //read
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>


#include "serv.h"
#include "db.h"

#define LISTENQ 5
#define THREAD_COUNT 20

typedef struct thread_struct {
	pthread_t thread_id;
	int socketfd;
} thread_s;


thread_s pool[THREAD_COUNT];
pthread_mutex_t accept_lock = PTHREAD_MUTEX_INITIALIZER;

void process_connection(int i)
{
}

void *conn_thread (void *arg)
{
	int listenfd, connfd;
	thread_s self;
	//struct sockaddr_in cliaddr;
	struct sockaddr cliaddr;
	socklen_t clisize;
	
	self = *(thread_s*)arg;
	listenfd = self.socketfd;
	
	for (;;) {
		pthread_mutex_lock(&accept_lock);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clisize);
		pthread_mutex_unlock(&accept_lock);
		
		if (connfd < 0) {
			perror("accept error");
		}
	
		process_connection(connfd);
		
		close(connfd);
	}
}

//type ip address(or not), port number.
void thread_make(long i, int fd){
	int status;
	pool[i].socketfd = fd;
	
	status = pthread_create(&pool[i].thread_id, NULL, conn_thread, &pool[i]);
	if(status != 0)
		perror("threads");
	return;
}




int main(int argc, const char * argv[]) {
    int listenfd,connfd,n;
    socklen_t clilen;
    struct sockaddr *cliaddr=NULL;
   
    char *mesbuff;
    Msgheader mesheader;
	const int on = 1;
    struct addrinfo hints, *res, *ressave;

	mesbuff=malloc(MAXMSG*sizeof(char)+1);
    
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ( (n = getaddrinfo(argv[1], argv[2], &hints, &res)) != 0) {
            fprintf(stderr, "tcp_listen error for %s, %s: %s", argv[1],
                             argv[2], gai_strerror(n));
		return -1;
	}
        ressave = res;

        do {
                listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (listenfd < 0)
                        continue;               /* error, try next one */

                setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
                if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
                        break;                  /* success */

                close(listenfd);        /* bind error, close and try next one */
        } while ( (res = res->ai_next) != NULL);

        if (res == NULL) {        /* errno from final socket() or bind() */
            fprintf(stderr, "tcp_listen error for %s, %s", argv[1], argv[2]);
            return -1;
        }

    
    if (listen(listenfd, LISTENQ) < 0) {
		perror("listen");
		return -1;
        }
    
    //printf("The ip address we are using is: ");
    //print_address(res);
    
    freeaddrinfo(ressave);
    for (n = 0;n<THREAD_COUNT;n++){
    	thread_make(n, listenfd);
    }
    
    // Join all threads
    for (n = 0; n < THREAD_COUNT; n++) {
    	void *thread_result;
    	int s;
    	
    	s = pthread_join(pool[n].thread_id, &thread_result);
    	
    	if (s != 0) {
    		perror("thread join");
    	}
    }
    
	return 0;
    
}

