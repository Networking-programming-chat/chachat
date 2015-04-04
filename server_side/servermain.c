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

#define MAX_MESSAGE_LEN 2048    // TODO: Make actual max len

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

void print_debug_info()
{
    // Print some server info
    
    cc_user *user_list;
    cc_channel *channel_list;
    
    user_list = get_all_users();
    print_user_list(user_list);
    
    channel_list = get_all_channels();
    print_channel_list(channel_list);
    
    free_cc_user(user_list);
    free_cc_channel(channel_list);
}

char * get_first_argument(const char *message, int *end)
{
    char *argument;
    int i;
    
    // Get argument length
    for (i = 2; message[i] != ' ' && message[i] != '\0'; ++i);
    
    *end = i;
    
    i -= 1;
    
    argument = malloc(sizeof(char) * i);
    strncpy(argument, message + 2, i);
    argument[i-1] = '\0';
    
    return argument;
}


void send_to_user(cc_user* sender, const char *message)
{
    int end;
    char *argument;
    
    //printf("%s\n", __func__);
    
    argument = get_first_argument(message, &end);
    
    // Get target user
    cc_user *target = get_user_by_nick(argument);
    
    if (target == NULL) {
        char response[128];
        printf("WARN user %s does not exist\n", argument);
        sprintf(response, "WARN user %s does not exist", argument);
        write_to_buffer(sender->user_id, response);
        return;
    }
    
    printf("send message to user id: %d, nick: %s from: %s\n", target->user_id, target->nick, sender->nick);
    
    write_to_buffer(target->user_id, message + end);
    part_channel(sender->nick, argument);
    free(argument);
}

void create_channel(cc_user* sender, const char *message)
{
    int end, status;
    char *argument;
    
    //printf("%s\n", __func__);
    
    argument = get_first_argument(message, &end);
    status = add_channel(argument);
    free(argument);
    
    if (status != SQLITE_OK) {
        char response[128];
        printf("WARN cannot create channel %s\n", argument);
        sprintf(response, "WARN cannot create channel %s", argument);
        write_to_buffer(sender->user_id, response);
    }
}

void send_to_channel(cc_user* sender, const char *message)
{
    int end;
    char *argument;
    cc_user *user_list, *user;
    
    //printf("%s\n", __func__);
    
    argument = get_first_argument(message, &end);
    
    // Get users of channel
    user_list = get_users_by_channel_name(argument);
    
    if (user_list == NULL) {
        char response[128];
        printf("WARN channel %s does not exist\n", argument);
        sprintf(response, "WARN channel %s does not exist or nobody has joined", argument);
        write_to_buffer(sender->user_id, response);
        return;
    }
    
    printf("send message to channel name: %s from: %s\n", argument, sender->nick);
    
    user = user_list;
    
    while (user != NULL) {
        write_to_buffer(user->user_id, message + end);
        user = user->next_user;
    }
    
    free_cc_user(user_list);
    free(argument);
}

void handle_channel_join(cc_user* sender, const char *message)
{
    int end, status;
    char *argument;
    
    //printf("%s\n", __func__);
    
    argument = get_first_argument(message, &end);
    status = join_channel(sender->nick, argument);
    free(argument);

    if (status != SQLITE_OK) {
        char response[128];
        printf("WARN cannot join channel %s, channel does not exist\n", argument);
        sprintf(response, "WARN cannot join channel %s, channel does not exist", argument);
        write_to_buffer(sender->user_id, response);
    }
}

void handle_channel_part(cc_user* sender, const char *message)
{
    int end, status;
    char *argument;
    
    //printf("%s\n", __func__);
    
    argument = get_first_argument(message, &end);
    status = part_channel(sender->nick, argument);
    free(argument);
    
    if (status != SQLITE_OK) {
        char response[128];
        printf("WARN cannot part channel %s\n", argument);
        sprintf(response, "WARN cannot part channel %s", argument);
        write_to_buffer(sender->user_id, response);
    }

}

void handle_nick_change(cc_user* sender, const char *message)
{
    //printf("%s\n", __func__);
    
    // Not implemented
    write_to_buffer(sender->user_id, "WARN Nick change not implemented.");
}

int handle_message(cc_user* sender, const char *message)
{
    char command;
    
    // Get command
    if (strnlen(message, MAX_MESSAGE_LEN) > 2 && message[1] == ' ') {
        command = message[0];
    }
    else {
        // Not proper message
        printf("Not a proper message: %s\n", message);
        return 0;
    }
    
    // Interpret command
    switch (command) {
        case 'q':
        case 'Q':
            // Quit command
            return -1;

        case 'm':
        case 'M':
            // Message for a user command
            send_to_user(sender, message);
            break;
            
        case 'u':
        case 'U':
            // Create a channel
            create_channel(sender, message);
            break;
            
        case 'c':
        case 'C':
            // Message for a channel command
            send_to_channel(sender, message);
            break;
            
        case 'j':
        case 'J':
            // Join a channel
            handle_channel_join(sender, message);
            break;
            
        case 'p':
        case 'P':
            // Part a channel
            handle_channel_part(sender, message);
            break;
            
        case 'n':
        case 'N':
            // Change nickname
            handle_nick_change(sender, message);
            break;
            
        case 'd':
        case 'D':
            // Server debug test
            print_debug_info();
            break;
            
        default:
            printf("Unknown command %c\n", command);
            break;
    }
    
    return 0;
}

void process_connection(int sockfd)
{
    // Wait for client nick
    
    char nickname[MAX_NICKLEN], response[50], incoming[1024];
    
    int i = 0;
    ssize_t n;
    struct timespec ts;
    cc_user *user;
    
    // Select stuff
    fd_set rset;

    
    for (;;) {
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
    }
    
    n = write(sockfd, response, sizeof(response));
    
    if (n < 0) {
        perror("write error (nickname final response)");
    }
    
    // Register message buffer
    new_buffer(user->user_id);
    
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
            n = recv(sockfd, incoming, 1023, 0);
            
            if (n < 0) {
                perror("recv error (client processing)");
                break;
            }
            
            incoming[n] = '\0';
            
            if (n > 0) {
                // Handle message sent by client
                
                n = handle_message(user, incoming);
                if (n < 0) {
                    // Quit message sent
                    break;
                }
                
                //write_to_buffer(user->user_id, incoming);
                //printf("Received message: %s\n", incoming);
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
    
    char *mesbuff;
    const int on = 1;
    struct addrinfo hints, *res, *ressave;
    
    // Check arguments
    if (argc != 3) {
        printf("Usage: %s [address] [port]\n", argv[0]);
        return 0;
    }
    
    // Setup database and msg buffers
    init_db();
    init_msg_buffers();
    
    mesbuff = malloc(MAXMSG*sizeof(char)+1);
    
    memset(&hints, '\0', sizeof(struct addrinfo));
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

