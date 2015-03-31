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
#include <stdarg.h> // va_
#include <errno.h>


#include "serv.h"
#include "msg_buffers.h"
#include "db.h"

#define LISTENQ 5
#define THREAD_COUNT 20

typedef struct thread_struct {
    pthread_t thread_id;
    int socketfd;
} thread_s;

// Function to test a condition (will kill program on error for now)
static void check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        
        // Iterate the arguments
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        
        // Print error message to stderr
        fprintf (stderr, ": %s\n", strerror(errno));
        
        // Failure
        exit (1);
    }
}


thread_s pool[THREAD_COUNT];
pthread_mutex_t accept_lock = PTHREAD_MUTEX_INITIALIZER;

void process_connection(int sockfd)
{
    // Wait for client nick
    
    char nickname[MAX_NICKLEN], response[50], incoming[1024];
    
    int i = 0, msec;
    ssize_t n;
    struct timeval tv;
    cc_user *user;
    
    for (;;) {
        n = read(sockfd, nickname, MAX_NICKLEN - 1);
        if (n < 0) {
            perror("read error (reading nickname)\n");
            return;
        }
        
        user = add_user_server(nickname, 0);
        
        if (user == NULL) {
            printf("Nickname %s already in use.\n", nickname);
            sprintf(response,"ERROR nickname already in use\n");
            
            n = write(sockfd,response,sizeof(response));
            
            if (n < 0) {
                perror("write error (nickname response)\n");
                return;
            }
            
            if (i++ < 3) {
                // Try again
                continue;
            }
            
            sprintf(response,"ERROR retry count full\n");
        }
        else {
            printf("Client nick: %s user id: %d\n", nickname, user->user_id);
            sprintf(response,"OK nickname set\n");
        }
        
        // Done here
        break;
    }
    
    n = write(sockfd, response, sizeof(response));
    
    if (n < 0) {
        perror("write error (nickname final response)\n");
    }
    
    // Register message buffer
    new_buffer(user->user_id);
    
    // Setup socket with timeout
    
    msec = 200;
    
    tv.tv_sec = 0;
    tv.tv_usec = msec * 1000;
    
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
    
    // Process client messages
    for (;;) {
        char * sendmessage;
        // Read message
        n = read(sockfd, incoming, 1023);
        if (n < 0) {
            perror("read error (client processing)\n");
            break;
        }
        
        incoming[1] = '\0';
        
        if (n > 0) {
            // Handle message sent by client
            write_to_buffer(user->user_id, incoming);
        }
        
        // Check for messages for client
        sendmessage = read_buffer_block(user->user_id);
        
        if (sendmessage != NULL) {
            // Send to client
            n = write(sockfd, sendmessage, strlen(sendmessage));
            
            free(sendmessage);
            
            if (n < 0) {
                perror("write error (message to client)\n");
                break;
            }
        }
    }

    free_cc_user(user);
    
    return;
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


void thread_make(long i, int fd){
    int status;
    pool[i].socketfd = fd;
    
    status = pthread_create(&pool[i].thread_id, NULL, conn_thread, &pool[i]);
    if(status != 0)
        perror("threads");
    return;
}


//type ip address(or not), port number.
int main(int argc, const char * argv[]) {
    int listenfd, n;
    
    char *mesbuff;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;
    
    // Check arguments
    if (argc != 3) {
        printf("Usage: %s [server] [port]\n", argv[0]);
        return 0;
    }
    
    // Setup database and msg buffers
    init_db();
    init_msg_buffers();
    
    mesbuff = malloc(MAXMSG*sizeof(char)+1);
    
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
    
    close_db();
    clear_all_msg_buffers();
    
    return 0;
    
}

