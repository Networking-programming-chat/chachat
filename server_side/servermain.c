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
//#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/select.h>
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
/*static void check (int test, const char * message, ...)
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
}*/


thread_s pool[THREAD_COUNT];
pthread_mutex_t accept_lock = PTHREAD_MUTEX_INITIALIZER;

void process_connection(int sockfd)
{
    // Wait for client nick
    
    char nickname[MAX_NICKLEN], incoming[1024];
    
    char *mesbuff;
    mesbuff=(char *)malloc(MAXMSG*sizeof(char)+1);
    Msgheader *mesheader;
    mesheader=(Msgheader *)malloc(HDRSIZE*sizeof(char)+1);
    
    int i = 0;
    ssize_t n;
    struct timespec ts;
    cc_user *user;
    
    // Select stuff
    fd_set rset;

    
   /* for (;;) {
        n = read(sockfd, nickname, MAX_NICKLEN - 1);
        if (n < 0) {
            perror("read error (reading nickname)");
            return;
        }
        
        user = add_user_server(nickname, 0);
        
        if (user == NULL) {
            printf("Nickname %s already in use.\n", nickname);
            sprintf(response,"ERROR nickname already in use\n");
            
            n = write(sockfd,response,sizeof(response));
            
            if (n < 0) {
                perror("write error (nickname response)");
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
    }*/
    
    if((client_nick(sockfd,nickname))<0)
        close(sockfd);
        return;
    
   // n = write(sockfd, response, sizeof(response));
    
    if (n < 0) {
        perror("write error (nickname final response)");
    }
    
    /*
    // Register message buffer
    new_buffer(user->user_id);
     */
    
    printf("start processing\n");
    
    // Process client messages
    for (;;) {
        char * sendmessage;
        
        FD_ZERO(&rset);
        FD_SET(sockfd, &rset);
        
        ts.tv_sec = 0;
        ts.tv_nsec = 200 * 1000000;
        
        i = pselect(sockfd + 1, &rset, NULL, NULL, &ts, 0);
        if (i < 0) {
            perror("select error");
        }
        
        if (FD_ISSET(sockfd, &rset)) {
            
            // Read message
           // n = recv(sockfd, incoming, 1023, 0);
            n = read_message(sockfd, mesbuff, mesheader);
            
            if (n < 0) {
                perror("recv error (client processing)");
                break;
            }
            
            if (n > 0) {
                /*
                // Handle message sent by client
                write_to_buffer(user->user_id, incoming);
                printf("Received message: %s\n", incoming);
                 */
                
                if (mesheader->firstbyte=='1') {//client sends private chat message
                    chatMessageHandle(sockfd, mesbuff, mesheader);
                    
                    
                }else if (mesheader->firstbyte=='2') {//client sends channel chat message
                    chanMessageHandle(sockfd,mesbuff,mesheader);
                    
                    
                } else if (mesheader->firstbyte=='3') {//client sends quit command
                    quitMessageHandle(sockfd,mesbuff,mesheader);
                    
                    
                }
            }
        }
        
        // Check for messages for client
        sendmessage = read_buffer(user->user_id);
        
        if (sendmessage != NULL) {
            printf("Will send to client: %s\n", sendmessage);
            
            n = strlen(sendmessage);
            
            // Send to client
            n = write(sockfd, sendmessage, n+1);
            
            free(sendmessage);
            
            if (n < 0) {
                perror("write error (message to client)\n");
                break;
            }
        }
    }
    
    remove_user(user->nick);

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
    
    //char *mesbuff;
    const int on = 1;
    
    // Check arguments
    if (argc==2)
        listenfd=serv_listen(NULL, argv[1]);
    else if (argc==3)
        listenfd=serv_listen(argv[1], argv[2]);
    else {
        fprintf(stderr, "usage: %s <host> <port#>\n ", argv[0]);
        return -1;
        
    }
    
    // Setup database and msg buffers
    init_db();
    init_msg_buffers();
    
    //mesbuff = malloc(MAXMSG*sizeof(char)+1);
    
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

